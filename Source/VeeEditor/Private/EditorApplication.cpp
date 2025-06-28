//
// Created by Square on 10/22/2024.
//

#include "EditorApplication.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

vee::EditorApplication::EditorApplication(platform::Window&& window)
    : Application(std::move(window)) {
    // TODO: This needs to be moved to the EditorRenderPass when the RenderGraph properly supports
    // resource initialization on a pass level.
    {
        auto& ctx = renderer_.get_ctx();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForVulkan(ctx.window->glfw_window, true);

        // NOTE: (from imgui example)
        // The example only requires a single combined image sampler descriptor for the font image
        // and only uses one descriptor set (for that) If you wish to load e.g. additional textures
        // you may need to alter pools sizes.
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
                return VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr(static_cast<VkInstance>(user_data), function_name);
            },
            ctx.instance.instance
        );
        ImGui_ImplVulkan_Init(&init_info);
        ImGui_ImplVulkan_CreateFontsTexture();
    }
}

void vee::EditorApplication::run() {
    engine_.init();
    while (!window_.should_close()) {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        engine_.tick();

        ImGui::ShowDemoWindow();
        renderer_.render();
        // We may bail out of rendering early (e.g. swapchain resizing)
        // We need to make sure we always end the ImGui frame.
        ImGui::EndFrame();

        window_.poll_events();
    }

    engine_.shutdown();

    // TODO: This needs to be moved to the EditorRenderPass when the RenderGraph properly supports
    // resource initialization on a pass level.
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    vee::log_info("Goodbye");
}
