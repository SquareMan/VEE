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


#include "Components/CameraComponent.hpp"

#include "Transform.h"

#include <glm/ext.hpp>

namespace vee {

CameraComponent::CameraComponent(float ortho_width, float ortho_height) {
    float half_width = ortho_width * 0.5f;
    float half_height = ortho_height * 0.5f;
    projection_matrix = glm::ortho(-half_width, half_width, -half_height, half_height);
}

CameraComponent::CameraComponent(float fov, float aspect_ratio, float z_near, float z_far) {
    projection_matrix = glm::perspective(fov, aspect_ratio, z_near, z_far);
}

glm::mat4x4 CameraComponent::calculate_view_projection(const Transform& transform) const {
    glm::mat4x4 view_mat = glm::inverse(transform.to_mat());
    return projection_matrix * view_mat;
}
} // namespace vee