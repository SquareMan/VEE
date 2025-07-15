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

namespace vee {
class Transform;

class CameraComponent {
public:
    glm::mat4x4 projection_matrix;

    CameraComponent(float ortho_width, float ortho_height);
    CameraComponent(float fov, float aspect_ratio, float z_near, float z_far);

    [[nodiscard]] glm::mat4x4 calculate_view_projection(const Transform&) const;
};

} // namespace vee
