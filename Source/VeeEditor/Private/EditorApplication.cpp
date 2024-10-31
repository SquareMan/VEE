//
// Created by Square on 10/22/2024.
//

#include "EditorApplication.hpp"

#include "ImguiRenderer.hpp"
#include "Renderer/RenderCtx.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <iostream>


vee::EditorApplication::EditorApplication(const platform::Window& window)
    : window_(window)
    , renderer_(std::make_shared<RenderCtx>(window)) {
    engine_.init();
}

void vee::EditorApplication::run() {
    renderer_.push_renderer<ImguiRenderer>();
    renderer_.init();
    while (!window_.should_close()) {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        engine_.tick();

        ImGui::ShowDemoWindow();
        ImGui::Render();
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
        renderer_.Render();

        window_.poll_events();
    }

    engine_.shutdown();

    std::cout << "Goodbye\n";
}

vee::Engine& vee::EditorApplication::get_engine() {
    return engine_;
}

vee::Renderer& vee::EditorApplication::get_renderer() {
    return renderer_;
}