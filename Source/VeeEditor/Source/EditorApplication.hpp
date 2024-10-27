//
// Created by Square on 10/22/2024.
//

#pragma once
#include "Engine.hpp"
#include "Platform/Window.hpp"
#include "Renderer.hpp"


namespace vee {
class EditorApplication {
public:
    EditorApplication(const platform::Window& window);
    void run();

private:
    Engine engine_;
    platform::Window window_;
    Renderer renderer_;
};
}; // namespace vee
