//
// Created by Square on 10/27/2024.
//

#include "ImguiRenderer.hpp"

#include "Platform/Window.hpp"
#include "Renderer/RenderCtx.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

namespace vee {
void ImguiRenderer::OnInit(std::shared_ptr<RenderCtx>& ctx) {
    ctx_ = ctx;

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(ctx_->window->glfw_window, true);

    // NOTE: (from imgui example)
    // The example only requires a single combined image sampler descriptor for the font image and
    // only uses one descriptor set (for that) If you wish to load e.g. additional textures you may
    // need to alter pools sizes.
    vk::DescriptorPoolSize pool_sizes[] = {
        {vk::DescriptorType::eCombinedImageSampler, 1},
    };

    vk::DescriptorPoolCreateInfo pool_info = {
        vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 1000, pool_sizes
    };
    vk::DescriptorPool pool = ctx_->device.createDescriptorPool(pool_info).value;

    vk::PipelineRenderingCreateInfo pipeline_info = {{}, ctx_->swapchain.format, {}, {}};
    ImGui_ImplVulkan_InitInfo init_info = {
        .Instance = ctx_->instance,
        .PhysicalDevice = ctx_->gpu,
        .Device = ctx_->device,
        .Queue = ctx_->graphics_queue,
        .DescriptorPool = pool,
        .MinImageCount = 3,
        .ImageCount = 3,
        .UseDynamicRendering = true,
        .PipelineRenderingCreateInfo = pipeline_info,
    };

    ImGui_ImplVulkan_LoadFunctions(
        [](const char* function_name, void* user_data) {
            return VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr(
                *static_cast<VkInstance*>(user_data), function_name
            );
        },
        &ctx_->instance.instance
    );
    ImGui_ImplVulkan_Init(&init_info);
    ImGui_ImplVulkan_CreateFontsTexture();
}

void ImguiRenderer::OnRender(vk::CommandBuffer cmd, uint32_t swapchain_idx) {
    vk::RenderingAttachmentInfo render_attachment = {
        ctx_->swapchain.image_views[swapchain_idx],
        vk::ImageLayout::eColorAttachmentOptimal,
        {},
        {},
        {},
        vk::AttachmentLoadOp::eLoad,
        vk::AttachmentStoreOp::eStore,
    };
    vk::RenderingInfo render_info = {
        {}, {{}, {ctx_->swapchain.width, ctx_->swapchain.height}}, 1, 0, render_attachment, {}, {}
    };
    cmd.beginRendering(render_info);
    { ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd); }
    cmd.endRendering();

    // swapchain image transition
    vulkan::transition_image(
        cmd,
        ctx_->swapchain.images[swapchain_idx],
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR
    );
}

void ImguiRenderer::OnDestroy() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
} // namespace vee