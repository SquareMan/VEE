//
// Created by Square on 10/22/2024.
//

#pragma once
#include "Engine/Engine.hpp"
#include "Platform/Window.hpp"
#include "Renderer.hpp"

#include <Engine/Service.hpp>


namespace vee {
class EditorApplication : public Service<EditorApplication> {
public:
    EditorApplication(const ConstructionToken&, const platform::Window& window);

    void run();

    Engine& get_engine();
    Renderer& get_renderer();

private:
    EditorApplication(const EditorApplication&) = delete;
    EditorApplication& operator=(const EditorApplication&) = delete;

    Engine engine_;
    platform::Window window_;
    // TODO: this needs to be in the Engine
    Renderer renderer_;
};
}; // namespace vee
