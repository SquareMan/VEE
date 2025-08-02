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


#include "Engine/Engine.hpp"

#include "Components/CameraComponent.hpp"
#include "Components/SpriteRendererComponent.hpp"
#include "Engine/Material.hpp"
#include "Engine/Texture.hpp"
#include "GameConfig.hpp"
#include "JobManager.hpp"
#include "Transform.h"

#include <GLFW/glfw3.h>
#include <tracy/Tracy.hpp>

namespace vee {

void Engine::init() {
    ZoneScoped;
    JobManager::init();

    if (g_game_info.game_init) {
        g_game_info.game_init();
    }
}

void Engine::shutdown() {
    JobManager::shutdown();
}


void Engine::tick() {
    ZoneScoped;
    const double new_time = glfwGetTime();
    delta_time_ = new_time - game_time_;
    game_time_ = new_time;

    if (g_game_info.game_tick) {
        g_game_info.game_tick();
    }
}


} // namespace vee