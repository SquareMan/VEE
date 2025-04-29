//
// Created by Square on 4/27/2025.
//

#include "Application.hpp"

#include "Renderer/GameRenderer.hpp"
#include "Renderer/RenderCtx.hpp"

vee::Application::Application(platform::Window&& window)
    : window_(std::move(window)) {
    RenderCtx::InitService(window_);

    renderer_.push_renderer(std::make_shared<GameRenderer>());
    renderer_.init();
    engine_.init();
}
vee::Application::Application(platform::Window&& window, std::vector<std::shared_ptr<IRenderer>> extra_renderers)
    : window_(std::move(window)) {
    RenderCtx::InitService(window_);
    renderer_.push_renderer(std::make_shared<GameRenderer>());
    for (std::shared_ptr<IRenderer>& renderer : extra_renderers) {
        renderer_.push_renderer(std::move(renderer));
    }
    renderer_.init();
    engine_.init();
}

void vee::Application::run() {
    while (!window_.should_close()) {
        engine_.tick();
        renderer_.Render();

        window_.poll_events();
    }

    engine_.shutdown();

    vee::log_info("Goodbye");
}

vee::Engine& vee::Application::get_engine() {
    return engine_;
}

vee::Renderer& vee::Application::get_renderer() {
    return renderer_;
}
