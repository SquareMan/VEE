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

// FIXME: Replace these with our own platform layer calls
extern "C" uint64_t _glfwPlatformGetTimerValue(void);
extern "C" uint64_t _glfwPlatformGetTimerFrequency(void);

namespace vee {

void Engine::init() {
    ZoneScoped;
    JobManager::init();

    start_time_ = _glfwPlatformGetTimerValue();

    if (g_game_info.game_init) {
        g_game_info.game_init();
    }
}

void Engine::shutdown() {
    JobManager::shutdown();
}


void Engine::tick() {
    ZoneScoped;
    const uint64_t now = _glfwPlatformGetTimerValue() - start_time_;
    delta_time_ = static_cast<double>(now - game_time_) / static_cast<double>(_glfwPlatformGetTimerFrequency());
    game_time_ = now;

    if (g_game_info.game_tick) {
        g_game_info.game_tick();
    }
}

double Engine::get_game_time() const {
    return static_cast<double>(game_time_) / static_cast<double>(_glfwPlatformGetTimerFrequency());
}


} // namespace vee