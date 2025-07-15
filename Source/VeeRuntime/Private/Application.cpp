//    Copyright 2025 Steven Casper
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.


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
