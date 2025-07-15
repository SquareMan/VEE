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

#include "Entity.h"

#include <entt/entt.hpp>

namespace vee {

class World {
public:
    [[nodiscard]] Entity spawn_entity();


    // FIXME: This should not be static or (probably) public. This is a necessary hack to allow
    // the renderer to find the camera and sprite entities but is going to be an issue that prevents
    // multiple worlds and parallelization of the game and renderer.
public:
    entt::registry entt_registry = entt::registry();
};
} // namespace vee
