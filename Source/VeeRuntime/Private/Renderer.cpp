//
// Created by Square on 9/28/2024.
//

#include "Renderer.hpp"

#include "Platform/Window.hpp"
#include "Renderer/RenderCtx.hpp"
#include "Vertex.hpp"

#include <cassert>
#include <chrono>
#include <vector>

namespace vee {
Renderer::Renderer(std::shared_ptr<RenderCtx>&& ctx)
    : ctx_(ctx) {}

Renderer::~Renderer() {
    // TODO: Cleanup everything
    std::ignore = ctx_->device.waitIdle();
    for (std::shared_ptr<IRenderer>& renderer : renderers_) {
        renderer->OnDestroy();
    }

#if _DEBUG
    static_cast<vk::Instance>(ctx_->instance).destroyDebugUtilsMessengerEXT(ctx_->debug_messenger_);
#endif

    for (CmdBuffer& command_buffer : ctx_->command_buffers.buffer) {
        ctx_->device.destroyFence(command_buffer.fence);
        ctx_->device.destroySemaphore(command_buffer.acquire_semaphore);
        ctx_->device.destroySemaphore(command_buffer.submit_semaphore);
    }
    ctx_->device.destroyCommandPool(ctx_->command_pool);
}

void Renderer::init() {
    for (std::shared_ptr<IRenderer>& r : renderers_) {
        r->OnInit(ctx_);
    }
}

void Renderer::Render() {
    CmdBuffer& command_buffer = ctx_->command_buffers.get_next();

    std::ignore = ctx_->device.waitForFences(command_buffer.fence, true, UINT64_MAX);
    uint32_t image_index = UINT32_MAX;
    {
        // Opt out of return value transformation to avoid asserting on
        // vk::Result::eErrorOutOfDateKHR
        const vk::Result result = ctx_->device.acquireNextImageKHR(
            ctx_->swapchain.handle, 0, command_buffer.acquire_semaphore, nullptr, &image_index
        );
        switch (result) {
        case vk::Result::eSuccess:
            break;
        case vk::Result::eTimeout:
            assert(false);
            return;
        case vk::Result::eNotReady:
            assert(false);
            return;
        case vk::Result::eSuboptimalKHR:
        case vk::Result::eErrorOutOfDateKHR:
            ctx_->recreate_swapchain();
        default:
            assert(false);
            return;
        }
    }
    assert(image_index != UINT32_MAX);
    std::ignore = ctx_->device.resetFences(command_buffer.fence);

    std::ignore = command_buffer.cmd.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    record_commands(command_buffer.cmd, [&](vk::CommandBuffer cmd) {
        {
            for (std::shared_ptr<IRenderer>& r : renderers_) {
                r->OnRender(cmd, image_index);
            }
        }
    });

    vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    const vk::SubmitInfo submit_info(
        command_buffer.acquire_semaphore,
        wait_stage,
        command_buffer.cmd,
        command_buffer.submit_semaphore
    );
    std::ignore = ctx_->graphics_queue.submit(submit_info, command_buffer.fence);


    const vk::PresentInfoKHR pi(
        command_buffer.submit_semaphore, ctx_->swapchain.handle, image_index
    );
    {
        // Opt out of return value transformation to avoid asserting on
        // vk::Result::eErrorOutOfDateKHR
        switch (ctx_->presentation_queue.presentKHR(&pi)) {
        case vk::Result::eSuccess:
            break;
        case vk::Result::eSuboptimalKHR:
        case vk::Result::eErrorOutOfDateKHR:
            ctx_->recreate_swapchain();
            break;
        default:
            assert(false);
        }
    }
}

void Renderer::record_commands(
    vk::CommandBuffer cmd, const std::function<void(vk::CommandBuffer cmd)>& func
) {
    std::ignore = cmd.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    func(cmd);
    std::ignore = cmd.end();
}
} // namespace vee