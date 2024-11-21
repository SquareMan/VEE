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
