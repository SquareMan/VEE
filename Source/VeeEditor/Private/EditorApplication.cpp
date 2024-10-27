//
// Created by Square on 10/22/2024.
//

#include "EditorApplication.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <iostream>


vee::EditorApplication::EditorApplication(const platform::Window& window)
    : engine_()
    , window_(window)
    , renderer_(window) {
    engine_.init();
}

void vee::EditorApplication::run() {
    while (!window_.should_close()) {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        engine_.tick();

        ImGui::ShowDemoWindow();
        ImGui::Render();
        renderer_.Render();

        window_.poll_events();
    }

    engine_.shutdown();

    std::cout << "Goodbye\n";
}