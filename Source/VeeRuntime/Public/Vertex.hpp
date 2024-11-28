//
// Created by Square on 10/3/2024.
//

#pragma once
#include "Renderer/VkUtil.hpp"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include <array>

namespace vee {
struct Vertex final {
    glm::vec2 pos;
    glm::vec3 color;
    glm::vec2 uv;

    static vk::VertexInputBindingDescription binding_description() {
        static constexpr vk::VertexInputBindingDescription binding_description(0, sizeof(Vertex), vk::VertexInputRate::eVertex);
        return binding_description;
    }

    static std::array<vk::VertexInputAttributeDescription, 3> attribute_descriptions() {
        static constexpr std::array<vk::VertexInputAttributeDescription, 3> attribute_descriptions{
            {{
                 0,
                 0,
                 vk::Format::eR32G32Sfloat,
                 offsetof(Vertex, pos),
             },
             {
                 1,
                 0,
                 vk::Format::eR32G32B32Sfloat,
                 offsetof(Vertex, color),
             },
             {
                 2,
                 0,
                 vk::Format::eR32G32Sfloat,
                 offsetof(Vertex, uv),
             }}
        }; // namespace vee

        return attribute_descriptions;
    }
};
} // namespace vee
