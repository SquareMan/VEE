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

#include "Renderer/Pipeline.hpp"

#include <expected>
#include <memory>

namespace vee {
namespace rdg {
class SceneRenderPass;
}
class Texture;
class Material {
public:
    struct CreateError {};
    static std::expected<std::shared_ptr<Material>, CreateError> create(const std::shared_ptr<Texture>& texture);

protected:
    std::shared_ptr<Texture> texture_;
    vulkan::Pipeline pipeline_;
    vk::DescriptorSet descriptor_set_;

    friend class GameRenderer;
    friend class rdg::SceneRenderPass;
};
} // namespace vee
