//
// Created by Square on 4/27/2025.
//

#pragma once

#include "Engine/Engine.hpp"
#include "Engine/Service.hpp"
#include "IApplication.hpp"
#include "Platform/Window.hpp"
#include "Renderer.hpp"


namespace vee {
class Application : public IApplication {
public:
    explicit Application(platform::Window&& window);
    Application(platform::Window&& window, std::vector<std::shared_ptr<IRenderer>> extra_renderers);

    void run() override;
    Engine& get_engine() override;
    Renderer& get_renderer() override;

protected:
    // Application(const Application&) = delete;
    // Application& operator=(const Application&) = delete;

    Engine engine_;
    platform::Window window_;
    // TODO: this needs to be in the Engine
    Renderer renderer_;
};
} // namespace vee
