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
