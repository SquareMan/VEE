//
// Created by Square on 10/3/2024.
//

#pragma once

#include <vector>

import vulkan_hpp;

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