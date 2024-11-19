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

private:
    World world_;
};
} // namespace vee
