//
// Created by Square on 1/13/2025.
//

module;
#include "Engine/Service.hpp"
#include <entt/entt.hpp>
#include <glm/glm.hpp>
export module Vee.Engine;

import Vee.Renderer;

namespace vee {
export class Transform {
public:
    glm::vec2 position = {0, 0};
    glm::vec2 scale = {1, 1};
    // radians
    float rotation = 0.0f;

    Transform() = default;

    Transform(const glm::vec2& position)
        : position(position)
        , scale({1, 1})
        , rotation(0) {}

    Transform(const glm::vec2& position, float rotation)
        : position(position)
        , scale({1, 1})
        , rotation(rotation) {}

    Transform(const glm::vec2& position, float rotation, const glm::vec2& scale)
        : position(position)
        , scale(scale)
        , rotation(rotation) {}

    [[nodiscard]] glm::mat4x4 to_mat() const;
};

export class Entity {
public:
    entt::registry* registry;
    entt::entity entt_entity;

    Entity(entt::registry* registry, const entt::entity entity)
        : registry(registry)
        , entt_entity(entity) {};

    template <typename T, typename... Args>
    T& add_component(Args&&... args) {
        assert(registry && "Entity has a null registry");
        return registry->emplace<T>(entt_entity, std::forward<Args>(args)...);
    }

    template <typename T>
    [[nodiscard]] T* get_component() {
        assert(registry && "Entity has a null registry");
        return registry->try_get<Transform>(entt_entity);
    }
};

export class World {
public:
    [[nodiscard]] Entity spawn_entity();


    // FIXME: This should not be static or (probably) public. This is a necessary hack to allow
    // the renderer to find the camera and sprite entities but is going to be an issue that prevents
    // multiple worlds and parallelization of the game and renderer.
public:
    entt::registry entt_registry = entt::registry();
};

export class Engine {
public:
    void init();
    void shutdown();

    // Run the main engine loop on the current thread. Will not return until the engine has finished
    // running
    void tick();

    [[nodiscard]] World& get_world() {
        return world_;
    };

    [[nodiscard]] const World& get_world() const {
        return world_;
    };

    [[nodiscard]] double get_game_time() const {
        return game_time_;
    }

private:
    double game_time_ = 0.0;
    World world_;
};

export class Application : public Service<Application> {
public:
    virtual ~Application() = default;

    virtual void run() = 0;

    virtual Engine& get_engine() = 0;
    virtual Renderer& get_renderer() = 0;
};
};