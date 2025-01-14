//
// Created by Square on 10/22/2024.
//

module;
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <iostream>
#include <entt/entt.hpp>
module Vee.Editor;

import Vee.Renderer;
import GameRenderer;

vee::EditorApplication::EditorApplication(const ConstructionToken&, const platform::Window& window)
    : window_(window) {
    RenderCtx::InitService(window);

    renderer_.push_renderer<GameRenderer>();
    renderer_.push_renderer<ImguiRenderer>();
    renderer_.init();
    engine_.init();
}

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

    std::cout << "Goodbye\n";
}

vee::Engine& vee::EditorApplication::get_engine() {
    return engine_;
}

vee::Renderer& vee::EditorApplication::get_renderer() {
    return renderer_;
}