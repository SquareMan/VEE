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
#include "JobManager.hpp"
#include "Transform.h"

#include <GLFW/glfw3.h>
#include <tracy/Tracy.hpp>

namespace vee {

void Engine::init() {
    JobManager::init();

    // TEMP: should be in game code
    World& world = get_world();

    Entity camera = world.spawn_entity();
    camera.add_component<Transform>();
    camera.add_component<CameraComponent>(640.f, 640.f);

    {
        Entity sprite = world.spawn_entity();
        sprite.add_component<Transform>(glm::vec2{0.0f, 150.f}, 0.f, glm::vec2{50.f, 50.f});

        std::shared_ptr<Texture> sprite_texture = Texture::create("Resources/cool2.png").value_or(nullptr);
        VASSERT(sprite_texture != nullptr);
        std::shared_ptr<Material> sprite_material = Material::create(sprite_texture).value_or(nullptr);
        VASSERT(sprite_material != nullptr);

        sprite.add_component<SpriteRendererComponent>(Sprite(sprite_material));
    }
    {
        Entity sprite = world.spawn_entity();
        sprite.add_component<Transform>(glm::vec2{}, 0.f, glm::vec2{100.f, 100.f});

        std::shared_ptr<Texture> sprite_texture = Texture::create("Resources/cool.png").value_or(nullptr);
        VASSERT(sprite_texture != nullptr);
        std::shared_ptr<Material> sprite_material = Material::create(sprite_texture).value_or(nullptr);
        VASSERT(sprite_material != nullptr);

        sprite.add_component<SpriteRendererComponent>(Sprite(sprite_material));
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

    World& world = get_world();
    auto view = world.entt_registry.view<SpriteRendererComponent, Transform>();
    auto time = static_cast<float>(get_game_time());
    for (auto [ent, spr, trans] : view.each()) {
        trans.position.x = ((time - static_cast<float>(static_cast<uint32_t>(time))) - 0.5f) * 2;
        trans.position.y = std::sin(time);
        trans.position *= 100.f;
    }
}


} // namespace vee