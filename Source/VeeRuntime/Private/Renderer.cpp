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
#include <vector>

namespace vee {
Renderer::Renderer() {}

Renderer::~Renderer() {
    // TODO: Cleanup everything
    RenderCtx& ctx = RenderCtx::GetService();
    std::ignore = ctx.device.waitIdle();
    for (std::shared_ptr<IRenderer>& renderer : renderers_) {
        renderer->on_destroy();
    }

#if _DEBUG
    static_cast<vk::Instance>(ctx.instance).destroyDebugUtilsMessengerEXT(ctx.debug_messenger_);
#endif

    for (CmdBuffer& command_buffer : ctx.command_buffers.buffer) {
        ctx.device.destroyFence(command_buffer.fence);
        ctx.device.destroySemaphore(command_buffer.acquire_semaphore);
        ctx.device.destroySemaphore(command_buffer.submit_semaphore);
    }
    ctx.device.destroyCommandPool(ctx.command_pool);
}

void Renderer::init() {
    for (std::shared_ptr<IRenderer>& r : renderers_) {
        r->on_init();
    }
}

void Renderer::Render() {
    RenderCtx& ctx = RenderCtx::GetService();
    CmdBuffer& command_buffer = ctx.command_buffers.get_next();

    std::ignore = ctx.device.waitForFences(command_buffer.fence, true, UINT64_MAX);
    uint32_t image_index = UINT32_MAX;
    {
        // Opt out of return value transformation to avoid asserting on
        // vk::Result::eErrorOutOfDateKHR
        const vk::Result result = ctx.device.acquireNextImageKHR(
            ctx.swapchain.handle, 0, command_buffer.acquire_semaphore, nullptr, &image_index
        );
        switch (result) {
        case vk::Result::eSuccess:
            break;
        case vk::Result::eSuboptimalKHR:
        case vk::Result::eErrorOutOfDateKHR:
            ctx.recreate_swapchain();
            return;
        default:
            VASSERT(false);
            return;
        }
    }
    VASSERT(image_index != UINT32_MAX, "failed to acquire image index");
    std::ignore = ctx.device.resetFences(command_buffer.fence);

    std::ignore = command_buffer.cmd.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    record_commands(command_buffer.cmd, [&](vk::CommandBuffer cmd) {
        for (std::shared_ptr<IRenderer>& r : renderers_) {
            r->on_render(cmd, image_index);
        }

        // Get the swapchain image ready to present.
        vulkan::transition_image(cmd, ctx.swapchain.images[image_index], vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR);
    });


    vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    const vk::SubmitInfo submit_info(
        command_buffer.acquire_semaphore, wait_stage, command_buffer.cmd, command_buffer.submit_semaphore
    );
    std::ignore = ctx.graphics_queue.submit(submit_info, command_buffer.fence);


    const vk::PresentInfoKHR pi(command_buffer.submit_semaphore, ctx.swapchain.handle, image_index);
    {
        // Opt out of return value transformation to avoid asserting on
        // vk::Result::eErrorOutOfDateKHR
        switch (ctx.presentation_queue.presentKHR(&pi)) {
        case vk::Result::eSuccess:
            break;
        case vk::Result::eSuboptimalKHR:
        case vk::Result::eErrorOutOfDateKHR:
            ctx.recreate_swapchain();
            break;
        default:
            VASSERT(false);
        }
    }
}

void Renderer::record_commands(vk::CommandBuffer cmd, const std::function<void(vk::CommandBuffer cmd)>& func) {
    std::ignore = cmd.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    func(cmd);
    std::ignore = cmd.end();
}
} // namespace vee