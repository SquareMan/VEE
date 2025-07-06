//
// Created by Square on 4/27/2025.
//

#include "Application.hpp"

#include "Renderer/RenderCtx.hpp"
#include "tracy/Tracy.hpp"

vee::Application::Application(platform::Window&& window)
    : window_(std::move(window))
    , renderer_(window_) {}

void vee::Application::run() {
    engine_.init();
    while (!window_.should_close()) {
        FrameMark;
        engine_.tick();
        renderer_.render();

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
