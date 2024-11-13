#include "Transform.h"

#include <numbers>

namespace vee {
glm::mat4x4 Transform::to_mat() const {
    return {
        scale.x * glm::vec4{std::cos(rotation), std::sin(rotation), 0, 0},
        scale.y
            * glm::
                vec4{std::cos(rotation + std::numbers::pi / 2), std::sin(rotation + std::numbers::pi / 2), 0, 0},
        {0, 0, 1, 0},
        {position.x, position.y, 0, 1}
    };
}
} // namespace vee