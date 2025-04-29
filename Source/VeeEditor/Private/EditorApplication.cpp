//
// Created by Square on 10/22/2024.
//

#include "EditorApplication.hpp"

#include "ImguiRenderer.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

vee::EditorApplication::EditorApplication(platform::Window&& window)
    : Application(std::move(window), {std::make_shared<ImguiRenderer>()}) {}

void vee::EditorApplication::run() {
    while (!window_.should_close()) {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        engine_.tick();

        ImGui::ShowDemoWindow();
        renderer_.Render();

        window_.poll_events();
    }

    engine_.shutdown();

    vee::log_info("Goodbye");
}