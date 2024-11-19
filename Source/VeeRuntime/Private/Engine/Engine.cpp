//
// Created by Square on 10/20/2024.
//

#include "Engine/Engine.hpp"

#include "Transform.h"

#include <Components/CameraComponent.hpp>

namespace vee {

void Engine::init() {
    // TEMP: should be in game code
    World& world = get_world();

    Entity camera = world.spawn_entity();
    camera.add_component<Transform>();
    camera.add_component<CameraComponent>(640.f,640.f);
}

void Engine::shutdown() {}


void Engine::tick() {
}

} // namespace vee