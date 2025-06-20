//
// Created by Square on 6/6/2025.
//

#include "RenderGraph/RenderGraph.hpp"

#include "Assert.hpp"
#include "Renderer/RenderCtx.hpp"
#include "RenderGraph/DirectSink.hpp"
#include "RenderGraph/ImageResource.hpp"
#include "RenderGraph/Pass.hpp"
#include "RenderGraph/Source.hpp"
#include "Vertex.hpp"

// TODO: Remove this after refactoring Tracy Image upload into this pass
#include "Engine/FrameImagePass.hpp"

#include <numbers>
#include <ranges>
#include <tracy/Tracy.hpp>

namespace vee::rdg {


struct null_deleter {
    template <typename T>
    void operator()(T*) const {}
};
RenderGraph::RenderGraph(std::unordered_map<PassHandle, std::unique_ptr<Pass>>&& passes, std::vector<PassHandle>&& pass_order, RenderCtx& render_ctx)
    : passes_(std::move(passes))
    , pass_order_(std::move(pass_order)) {
    framebuffer_ = std::make_shared<ImageResource>();

    // FIXME: Ownership and/or reference strategy is weird here.
    vertex_buffer_ = std::shared_ptr<Buffer>(&render_ctx.vertex_buffer, null_deleter());
    index_buffer_ = std::shared_ptr<Buffer>(&render_ctx.index_buffer, null_deleter());

    global_sinks_.insert({"framebuffer", DirectSink<ImageResource>::make(framebuffer_)});
    global_sinks_.insert({"vertex_buffer", DirectSink<Buffer>::make(vertex_buffer_)});
    global_sinks_.insert({"index_buffer", DirectSink<Buffer>::make(index_buffer_)});

    // TODO: Topological sort before this, validate topology, etc.
    for (auto& pass_handle : pass_order_) {
        auto& pass = passes_.at(pass_handle);
        for (const auto& sink : pass->iterate_sinks()) {
            sink->init(render_ctx);
        }
        for (const auto& source : pass->iterate_sources()) {
            source->resolve(*this);
        }
    }
}

RenderGraph::~RenderGraph() = default;
void RenderGraph::execute(RenderCtx& render_ctx) const {
    ZoneScoped;

    CmdBuffer& command_buffer = render_ctx.command_buffers.get_next();

    uint32_t image_index = UINT32_MAX;
    {
        ZoneScopedN("acquire swapchain");
        std::ignore = render_ctx.device.waitForFences(command_buffer.fence, true, UINT64_MAX);

        // Opt out of return value transformation to avoid asserting on
        // vk::Result::eErrorOutOfDateKHR
        const vk::Result result = render_ctx.device.acquireNextImageKHR(
            render_ctx.swapchain.handle, 0, command_buffer.acquire_semaphore, nullptr, &image_index
        );
        switch (result) {
        case vk::Result::eSuccess:
            break;
        case vk::Result::eSuboptimalKHR:
        case vk::Result::eErrorOutOfDateKHR:
            render_ctx.recreate_swapchain();
            return;
        default:
            VASSERT(false);
            return;
        }
    }
    VASSERT(image_index != UINT32_MAX, "failed to acquire image index");
    std::ignore = render_ctx.device.resetFences(command_buffer.fence);

    // Update ring-buffered resources
    const Swapchain& swapchain = render_ctx.swapchain;
    framebuffer_->image = swapchain.images[image_index];
    framebuffer_->view = swapchain.image_views[image_index];
    framebuffer_->width = swapchain.width;
    framebuffer_->height = swapchain.height;


    vk::CommandBuffer cmd = command_buffer.cmd;
    std::ignore = cmd.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    std::ignore = cmd.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    {
        const vk::ImageMemoryBarrier2 image_barrier = {
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::AccessFlagBits2::eNone,
            vk::PipelineStageFlagBits2::eAllGraphics,
            vk::AccessFlagBits2::eColorAttachmentWrite,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal,
            {},
            {},
            framebuffer_->image,
            {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
        };

        vk::DependencyInfo dependency_info;
        dependency_info.setImageMemoryBarriers(image_barrier);
        cmd.pipelineBarrier2(dependency_info);
    }
    for (const auto& pass_handle : pass_order_) {
        const auto& pass = passes_.at(pass_handle);
        // TODO: Insert image transitions based on Sink's current usage
        // for (const auto& sink : pass->sinks) {
        // }
        pass->execute(cmd);
    }
    {
        const vk::ImageMemoryBarrier2 image_barrier[] = {
            {vk::PipelineStageFlagBits2::eColorAttachmentOutput,
             vk::AccessFlagBits2::eColorAttachmentWrite,
             vk::PipelineStageFlagBits2::eNone,
             vk::AccessFlagBits2::eNone,
             vk::ImageLayout::eColorAttachmentOptimal,
             vk::ImageLayout::ePresentSrcKHR,
             {},
             {},
             framebuffer_->image,
             {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}},
        };

        vk::DependencyInfo dependency_info;
        dependency_info.setImageMemoryBarriers(image_barrier);
        cmd.pipelineBarrier2(dependency_info);
    }
    std::ignore = cmd.end();

    {
        ZoneScopedN("Submit");
        vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        const vk::SubmitInfo submit_info(
            command_buffer.acquire_semaphore, wait_stage, command_buffer.cmd, command_buffer.submit_semaphore
        );
        std::ignore = render_ctx.graphics_queue.submit(submit_info, command_buffer.fence);
    }


    const vk::PresentInfoKHR pi(command_buffer.submit_semaphore, render_ctx.swapchain.handle, image_index);
    {
        ZoneScopedN("Present");
        // Opt out of return value transformation to avoid asserting on
        // vk::Result::eErrorOutOfDateKHR
        switch (render_ctx.presentation_queue.presentKHR(&pi)) {
        case vk::Result::eSuccess:
            break;
        case vk::Result::eSuboptimalKHR:
        case vk::Result::eErrorOutOfDateKHR:
            render_ctx.recreate_swapchain();
            return;
        default:
            VASSERT(false);
        }
    }
#if defined(TRACY_ENABLE) && !(TRACY_NO_FRAME_IMAGE)
    {
        ZoneScopedN("Upload Frame Image");
        // FIXME: Currently this prevents us from having multiple frames in flight.
        // This needs to be done asynchronously so as not to block the render loop. Tracy  provides
        // an offset parameter here for signaling how many frames behind the data is once it's ready
        // FIXME: This also needs to be controlled from the Debug/FrameImagePass somehow
        Sink* debug_sink = find_sink({"frame_image", "copy_buffer"});
        void* image_data = dynamic_cast<CopyBufferSink*>(debug_sink)->target->mem;
        std::ignore = render_ctx.device.waitForFences(command_buffer.fence, true, UINT64_MAX);
        FrameImage(image_data, DebugScreen::WIDTH, DebugScreen::HEIGHT, 0, false);
    }
#endif
}

Sink* RenderGraph::find_sink(SinkRef ref) const {
    if (ref.pass == GLOBAL) {
        auto sink_entry = global_sinks_.find(ref.sink);
        if (sink_entry == global_sinks_.end()) {
            log_error("Global sink {} does not exist", ref.sink.hash);
            return nullptr;
        }
        return sink_entry->second.get();
    }

    auto pass_entry = passes_.find(ref.pass);
    if (pass_entry == passes_.end()) {
        log_error("pass {} does not exist", ref.pass.hash);
        return nullptr;
    }
    return pass_entry->second->find_sink(ref.sink);
}
} // namespace vee::rdg