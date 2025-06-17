//
// Created by Square on 4/27/2025.
//

#pragma once

#include "Engine/Engine.hpp"
#include "IApplication.hpp"
#include "Platform/Window.hpp"
#include "Renderer.hpp"


namespace vee {
class Application : public IApplication {
public:
    explicit Application(platform::Window&& window);

    void run() override;
    Engine& get_engine() override;
    Renderer& get_renderer() override;

protected:
    Engine engine_;
    platform::Window window_;
    // TODO: this needs to be in the Engine
    Renderer renderer_;
};
} // namespace vee
