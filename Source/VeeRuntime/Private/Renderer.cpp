//
// Created by Square on 9/28/2024.
//

#include "Renderer.hpp"

#include "Assert.hpp"
#include "Platform/Window.hpp"
#include "Renderer/RenderCtx.hpp"
#include "Vertex.hpp"

#include <chrono>
#include <entt/locator/locator.hpp>
#include <stb_image_write.h>
#include <tracy/Tracy.hpp>
#include <vector>

namespace vee {
Renderer::Renderer(const platform::Window& window)
    : render_ctx_(window) {}

Renderer::~Renderer() {
    // TODO: Cleanup everything
    std::ignore = render_ctx_.device.waitIdle();
    for (std::shared_ptr<IRenderer>& renderer : renderers_) {
        renderer->on_destroy();
    }

#if defined(TRACY_ENABLE) && !defined(TRACY_NO_FRAME_IMAGE)
    for (DebugScreen& debug : debug_screens_) {
        render_ctx_.allocator.unmapMemory(debug.buf.allocation);
        render_ctx_.allocator.destroyBuffer(debug.buf.buffer, debug.buf.allocation);
        render_ctx_.allocator.destroyImage(debug.image, debug.alloc);
        debug.mem = nullptr;
    }
#endif

#if _DEBUG
    static_cast<vk::Instance>(render_ctx_.instance).destroyDebugUtilsMessengerEXT(render_ctx_.debug_messenger_);
#endif

    for (CmdBuffer& command_buffer : render_ctx_.command_buffers.buffer) {
        render_ctx_.device.destroyFence(command_buffer.fence);
        render_ctx_.device.destroySemaphore(command_buffer.acquire_semaphore);
        render_ctx_.device.destroySemaphore(command_buffer.submit_semaphore);
    }
    render_ctx_.device.destroyCommandPool(render_ctx_.command_pool);
}

void Renderer::init() {
#if defined(TRACY_ENABLE) && !defined(TRACY_NO_FRAME_IMAGE)
    for (DebugScreen& debug : debug_screens_) {
        constexpr auto FMT = vk::Format::eR8G8B8A8Srgb;
        constexpr vk::ImageCreateInfo image_info = {
            {}, vk::ImageType::e2D, FMT, {DebugScreen::WIDTH, DebugScreen::HEIGHT, 1}, 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc
        };
        constexpr vma::AllocationCreateInfo allocation_info = {{}, vma::MemoryUsage::eAuto, {}, vk::MemoryPropertyFlagBits::eDeviceLocal};

        const auto image_alloc = render_ctx_.allocator.createImage(image_info, allocation_info).value;
        debug.image = image_alloc.first;
        debug.alloc = image_alloc.second;

        vk::BufferCreateInfo buffer_info({}, DebugScreen::WIDTH * DebugScreen::HEIGHT * 4, vk::BufferUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive);
        auto [buf, alloc] =
            render_ctx_.allocator
                .createBuffer(buffer_info, {vma::AllocationCreateFlagBits::eHostAccessSequentialWrite, vma::MemoryUsage::eAuto})
                .value;
        new (&debug.buf) Buffer(buf, alloc, render_ctx_.allocator);
        debug.mem = render_ctx_.allocator.mapMemory(debug.buf.allocation).value;
    }
#endif

    for (std::shared_ptr<IRenderer>& r : renderers_) {
        r->on_init(render_ctx_);
    }
}
RenderCtx& Renderer::get_ctx() {
    return render_ctx_;
}

void Renderer::Render() {
    FrameMark;
    ZoneScoped;
    CmdBuffer& command_buffer = render_ctx_.command_buffers.get_next();

    uint32_t image_index = UINT32_MAX;
    {
        ZoneScopedN("acquire swapchain");
        std::ignore = render_ctx_.device.waitForFences(command_buffer.fence, true, UINT64_MAX);

        // Opt out of return value transformation to avoid asserting on
        // vk::Result::eErrorOutOfDateKHR
        const vk::Result result = render_ctx_.device.acquireNextImageKHR(
            render_ctx_.swapchain.handle, 0, command_buffer.acquire_semaphore, nullptr, &image_index
        );
        switch (result) {
        case vk::Result::eSuccess:
            break;
        case vk::Result::eSuboptimalKHR:
        case vk::Result::eErrorOutOfDateKHR:
            render_ctx_.recreate_swapchain();
            return;
        default:
            VASSERT(false);
            return;
        }
    }
    VASSERT(image_index != UINT32_MAX, "failed to acquire image index");
    std::ignore = render_ctx_.device.resetFences(command_buffer.fence);

    std::ignore = command_buffer.cmd.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    record_commands(command_buffer.cmd, [&](vk::CommandBuffer cmd) {
        for (std::shared_ptr<IRenderer>& r : renderers_) {
            r->on_render(cmd, image_index);
        }

#if defined(TRACY_ENABLE) && !defined(TRACY_NO_FRAME_IMAGE)
        // For Tracy, we need to save a copy of the framebuffer to the CPU. However, it needs to be
        // downscaled for better transfer performance, so we will need to blit to an intermediate
        // image to copy from
        const DebugScreen& debug = debug_screens_[image_index];
        vulkan::transition_image(cmd, debug.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        vulkan::transition_image(cmd, render_ctx_.swapchain.images[image_index], vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eTransferSrcOptimal);
        const auto blit = vk::ImageBlit2(
            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
            {vk::Offset3D{0, 0, 0},
             vk::Offset3D{
                 static_cast<int32_t>(render_ctx_.swapchain.width),
                 static_cast<int32_t>(render_ctx_.swapchain.height),
                 1
             }},
            {vk::ImageAspectFlagBits::eColor, 0, 0, 1},
            {vk::Offset3D{0, 0, 0}, vk::Offset3D{DebugScreen::WIDTH, DebugScreen::HEIGHT, 1}}
        );
        auto info = vk::BlitImageInfo2(
            render_ctx_.swapchain.images[image_index], vk::ImageLayout::eTransferSrcOptimal, debug.image, vk::ImageLayout::eTransferDstOptimal, blit, vk::Filter::eNearest
        );
        cmd.blitImage2(info);
        vulkan::transition_image(cmd, debug.image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal);
        cmd.copyImageToBuffer(
            debug.image,
            vk::ImageLayout::eTransferSrcOptimal,
            debug.buf.buffer,
            vk::BufferImageCopy(0, DebugScreen::WIDTH, DebugScreen::HEIGHT, {vk::ImageAspectFlagBits::eColor, 0, 0, 1}, {0, 0, 0}, {DebugScreen::WIDTH, DebugScreen::HEIGHT, 1})
        );

        vulkan::transition_image(cmd, render_ctx_.swapchain.images[image_index], vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::ePresentSrcKHR);
#else
        // Get the swapchain image ready to present.
        vulkan::transition_image(cmd, render_ctx_.swapchain.images[image_index], vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR);
#endif
    });


    vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    const vk::SubmitInfo submit_info(
        command_buffer.acquire_semaphore, wait_stage, command_buffer.cmd, command_buffer.submit_semaphore
    );
    std::ignore = render_ctx_.graphics_queue.submit(submit_info, command_buffer.fence);


    const vk::PresentInfoKHR pi(command_buffer.submit_semaphore, render_ctx_.swapchain.handle, image_index);
    {
        // Opt out of return value transformation to avoid asserting on
        // vk::Result::eErrorOutOfDateKHR
        switch (render_ctx_.presentation_queue.presentKHR(&pi)) {
        case vk::Result::eSuccess:
            break;
        case vk::Result::eSuboptimalKHR:
        case vk::Result::eErrorOutOfDateKHR:
            render_ctx_.recreate_swapchain();
            return;
        default:
            VASSERT(false);
        }
    }
#if defined(TRACY_ENABLE) && !(TRACY_NO_FRAME_IMAGE)
    // FIXME: Currently this prevents us from having multiple frames in flight.
    // This needs to be done asynchronously so as not to block the render loop. Tracy provides an
    // offset parameter here for signaling how many frames behind the data is once it's ready
    const DebugScreen& debug = debug_screens_[image_index];
    std::ignore = render_ctx_.device.waitForFences(command_buffer.fence, true, UINT64_MAX);
    FrameImage(debug.mem, DebugScreen::WIDTH, DebugScreen::HEIGHT, 0, false);
#endif
}

void Renderer::record_commands(vk::CommandBuffer cmd, const std::function<void(vk::CommandBuffer cmd)>& func) {
    std::ignore = cmd.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    func(cmd);
    std::ignore = cmd.end();
}
} // namespace vee