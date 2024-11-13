#pragma once
#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"

namespace vee {
class Transform {
public:
    glm::vec2 position = {0, 0};
    glm::vec2 scale = {1, 1};
    float rotation = 0.0f;

    Transform() = default;

    Transform(const glm::vec2& position)
        : position(position)
        , scale({1, 1})
        , rotation(0) {}

    Transform(const glm::vec2& position, const glm::vec2& scale, float rotation)
        : position(position)
        , scale(scale)
        , rotation(rotation) {}

    [[nodiscard]] glm::mat4x4 to_mat() const;
};
} // namespace vee