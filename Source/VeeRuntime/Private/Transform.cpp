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

#include "Transform.h"

#include <numbers>

namespace vee {
glm::mat4x4 Transform::to_mat() const {
    float c_theta, s_theta;
    c_theta = std::cos(rotation);
    s_theta = std::sin(rotation);
    return {
        scale.x * glm::vec4{c_theta, s_theta, 0, 0},
        scale.y * glm::vec4{-s_theta, c_theta, 0, 0},
        {0, 0, 1, 0},
        {position.x, position.y, 0, 1}
    };
}
} // namespace vee