//
// Created by Square on 1/13/2025.
//

module;
#include "Engine/Service.hpp"
#include "Platform/Window.hpp"
#include <entt/entt.hpp>
export module Vee.Editor;

import vulkan_hpp;
import Vee.Engine;
import Vee.Renderer;

namespace vee {
export class EditorApplication : public Application {
public:
    EditorApplication(const ConstructionToken&, const platform::Window& window);

    void run() override;

    Engine& get_engine() override;
    Renderer& get_renderer() override;

private:
    EditorApplication(const EditorApplication&) = delete;
    EditorApplication& operator=(const EditorApplication&) = delete;

    Engine engine_;
    platform::Window window_;
    // TODO: this needs to be in the Engine
    Renderer renderer_;
};

export class ImguiRenderer : public vee::IRenderer {
public:
    void on_init() override;
    void on_render(vk::CommandBuffer cmd, uint32_t swapchain_idx) override;
    void on_destroy() override;
};
}; // namespace vee
