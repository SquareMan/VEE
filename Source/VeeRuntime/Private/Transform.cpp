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