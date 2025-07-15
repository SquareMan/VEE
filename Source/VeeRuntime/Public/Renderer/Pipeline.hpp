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

#define VK_NO_PROTOTYPES
#include <vector>
#include <vulkan/vulkan.hpp>

#pragma once

namespace vee::vulkan {
class Shader;

class Pipeline final {
public:
    vk::PipelineLayout layout;
    vk::Pipeline pipeline;
    vk::DescriptorSetLayout descriptor_set_layout;
};

class PipelineBuilder final {
public:
    Pipeline build(vk::Device device);
    PipelineBuilder& with_cache(vk::PipelineCache cache);
    PipelineBuilder& with_shader(const Shader& shader);
    PipelineBuilder& with_binding(const vk::DescriptorSetLayoutBinding& binding);

private:
    vk::PipelineCache m_cache;

    std::vector<vk::PipelineShaderStageCreateInfo> pipeline_shader_stage_infos;
    std::vector<vk::DescriptorSetLayoutBinding> descriptor_set_layout_bindings;
};

} // namespace vee::vulkan