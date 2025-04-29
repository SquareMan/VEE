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
Renderer::Renderer(const platform::Window& window)
    : render_ctx_(window) {}

Renderer::~Renderer() {
    // TODO: Cleanup everything
    std::ignore = render_ctx_.device.waitIdle();
    for (std::shared_ptr<IRenderer>& renderer : renderers_) {
        renderer->on_destroy();
    }

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
    for (std::shared_ptr<IRenderer>& r : renderers_) {
        r->on_init(render_ctx_);
    }
}
RenderCtx& Renderer::get_ctx() {
    return render_ctx_;
}

void Renderer::Render() {
    CmdBuffer& command_buffer = render_ctx_.command_buffers.get_next();

    std::ignore = render_ctx_.device.waitForFences(command_buffer.fence, true, UINT64_MAX);
    uint32_t image_index = UINT32_MAX;
    {
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

        // Get the swapchain image ready to present.
        vulkan::transition_image(cmd, render_ctx_.swapchain.images[image_index], vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR);
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