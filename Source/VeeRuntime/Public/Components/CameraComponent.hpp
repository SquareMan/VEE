//
// Created by Square on 11/17/2024.
//

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
