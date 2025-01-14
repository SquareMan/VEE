//
// Created by Square on 10/27/2024.
//
module;
#include "Platform/Window.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <entt/entt.hpp>
module Vee.Editor;

import Vee.Renderer;
import VkUtil;

namespace vee {
void ImguiRenderer::on_init() {
    auto& ctx = RenderCtx::GetService();

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(ctx.window->glfw_window, true);

    // NOTE: (from imgui example)
    // The example only requires a single combined image sampler descriptor for the font image and
    // only uses one descriptor set (for that) If you wish to load e.g. additional textures you may
    // need to alter pools sizes.
    vk::DescriptorPoolSize pool_sizes[] = {
        {vk::DescriptorType::eCombinedImageSampler, 1},
    };

    vk::DescriptorPoolCreateInfo pool_info = {vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 1000, pool_sizes};
    vk::DescriptorPool pool = ctx.device.createDescriptorPool(pool_info).value;

    vk::PipelineRenderingCreateInfo pipeline_info = {{}, ctx.swapchain.format, {}, {}};
    ImGui_ImplVulkan_InitInfo init_info = {
        .Instance = ctx.instance,
        .PhysicalDevice = ctx.gpu,
        .Device = ctx.device,
        .Queue = ctx.graphics_queue,
        .DescriptorPool = pool,
        .MinImageCount = 3,
        .ImageCount = 3,
        .UseDynamicRendering = true,
        .PipelineRenderingCreateInfo = pipeline_info,
    };

    ImGui_ImplVulkan_LoadFunctions(
        [](const char* function_name, void* user_data) {
            return vk::defaultDispatchLoaderDynamic.vkGetInstanceProcAddr(static_cast<VkInstance>(user_data), function_name);
        },
        ctx.instance.instance
    );
    ImGui_ImplVulkan_Init(&init_info);
    ImGui_ImplVulkan_CreateFontsTexture();
}

void ImguiRenderer::on_render(vk::CommandBuffer cmd, uint32_t swapchain_idx) {
    auto& ctx = RenderCtx::GetService();

    ImGui::Render();
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    vk::RenderingAttachmentInfo render_attachment = {
        ctx.swapchain.image_views[swapchain_idx],
        vk::ImageLayout::eColorAttachmentOptimal,
        {},
        {},
        {},
        vk::AttachmentLoadOp::eLoad,
        vk::AttachmentStoreOp::eStore,
    };
    vk::RenderingInfo render_info = {{}, {{}, {ctx.swapchain.width, ctx.swapchain.height}}, 1, 0, render_attachment, {}, {}};
    cmd.beginRendering(render_info);
    { ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd); }
    cmd.endRendering();

    // swapchain image transition
    vulkan::transition_image(cmd, ctx.swapchain.images[swapchain_idx], vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR);
}

void ImguiRenderer::on_destroy() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
} // namespace vee