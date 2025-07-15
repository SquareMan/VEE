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

    [[nodiscard]] double get_delta_time() const {
        return delta_time_;
    };

private:
    double game_time_ = 0.0;
    double delta_time_ = 0.0;
    World world_;
};
} // namespace vee
