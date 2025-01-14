//
// Created by Square on 10/20/2024.
//

#include "Engine/Engine.hpp"
#include "Renderer/Image.hpp"
#include "Engine/Sprite.hpp"
#include "Transform.h"

#include <glfw/glfw3.h>

import CameraComponent;
import SpriteRendererComponent;

namespace vee {

void Engine::init() {
    // TEMP: should be in game code
    World& world = get_world();

    Entity camera = world.spawn_entity();
    camera.add_component<Transform>();
    camera.add_component<CameraComponent>(640.f, 640.f);

    Entity sprite = world.spawn_entity();
    sprite.add_component<Transform>(glm::vec2{}, 0.f, glm::vec2{100.f, 100.f});
    std::shared_ptr<vee::Image>& checkerboard = entt::locator<std::shared_ptr<vee::Image>>::value();
    Sprite spr(checkerboard);
    sprite.add_component<SpriteRendererComponent>(spr);

    sprite = world.spawn_entity();
    sprite.add_component<Transform>(glm::vec2{}, 30.f, glm::vec2{100.f, 100.f});
    sprite.add_component<SpriteRendererComponent>(spr);
}

void Engine::shutdown() {}


void Engine::tick() {
    game_time_ = glfwGetTime();

    World& world = get_world();
    auto view = world.entt_registry.view<SpriteRendererComponent, Transform>();
    auto time = static_cast<float>(get_game_time());
    for (auto [ent, spr, trans] : view.each()) {
        trans.position.x = ((time - static_cast<uint32_t>(time)) - 0.5f) * 2;
        trans.position.y = std::sin(time);
        trans.position *= 100.f;
    }
}

} // namespace vee