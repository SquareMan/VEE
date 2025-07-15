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
