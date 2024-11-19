#pragma once
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
        , entt_entity(entity){};

    template <typename T, typename... Args> T& add_component(Args&&... args) {
        assert(registry && "Entity has a null registry");
        return registry->emplace<T>(entt_entity, std::forward<Args>(args)...);
    }

    template <typename T> [[nodiscard]] T* get_component() {
        assert(registry && "Entity has a null registry");
        return registry->try_get<Transform>(entt_entity);
    }
};
} // namespace vee
