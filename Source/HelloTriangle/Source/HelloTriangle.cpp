
#include "Platform/Window.hpp"
#include "Renderer.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <iostream>

int main(int argc, char* argv[]) {
    const vee::platform::Window window(640, 640);
    vee::Renderer renderer(window);

    while (!window.should_close()) {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();
        ImGui::Render();

        renderer.Render();
        window.poll_events();
    }

    std::cout << "Goodbye\n";

    return 0;
}