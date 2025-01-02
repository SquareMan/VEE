//
// Created by Square on 11/17/2024.
//

module;
#include "Transform.h"

#include "glm/mat4x4.hpp"

export module CameraComponent;

namespace vee {

export class CameraComponent {
public:
    glm::mat4x4 projection_matrix;

    CameraComponent(float ortho_width, float ortho_height);
    CameraComponent(float fov, float aspect_ratio, float z_near, float z_far);

    [[nodiscard]] glm::mat4x4 calculate_view_projection(const Transform&) const;
};

} // namespace vee
