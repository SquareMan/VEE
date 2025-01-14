#include "Engine/World.h"

vee::Entity vee::World::spawn_entity() {
    const entt::entity ent = entt_registry.create();
    return {&entt_registry, ent};
}
