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
#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"

namespace vee {
class Transform {
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
} // namespace vee