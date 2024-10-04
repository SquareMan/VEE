//
// Created by Square on 10/3/2024.
//

#pragma once
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#define VK_NO_PROTOTYPES
#include <array>
#include <vulkan/vulkan_core.h>

namespace Vee {
struct Vertex final {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription binding_description() {
        static constexpr VkVertexInputBindingDescription binding_description = {
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        };
        return binding_description;
    }

    static std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions() {
        static constexpr std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions{
            {{
                 .location = 0,
                 .binding = 0,
                 .format = VK_FORMAT_R32G32_SFLOAT,
                 .offset = offsetof(Vertex, pos),
             },
             {
                 .location = 1,
                 .binding = 0,
                 .format = VK_FORMAT_R32G32B32_SFLOAT,
                 .offset = offsetof(Vertex, color),
             }}
        };

        return attribute_descriptions;
    }
};
} // namespace Vee
