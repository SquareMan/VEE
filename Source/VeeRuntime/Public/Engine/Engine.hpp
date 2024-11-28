//
// Created by Square on 10/20/2024.
//

#pragma once
#include "World.h"

namespace vee {

class Engine {
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
} // namespace vee
