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
#include "Assert.hpp"
#include "Transform.h"

#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>
#include <optional>


namespace vee {
class Entity {
public:
    entt::registry* registry;
    entt::entity entt_entity;

    Entity(entt::registry* registry, const entt::entity entity)
        : registry(registry)
        , entt_entity(entity) {};

    template <typename T, typename... Args>
    T& add_component(Args&&... args) {
        VASSERT(registry, "Entity has a null registry");
        return registry->emplace<T>(entt_entity, std::forward<Args>(args)...);
    }

    template <typename T>
    [[nodiscard]] T* get_component() {
        VASSERT(registry, "Entity has a null registry");
        return registry->try_get<Transform>(entt_entity);
    }
};
} // namespace vee
