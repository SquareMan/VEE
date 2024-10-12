//
// Created by Square on 10/3/2024.
//

#pragma once

#define VK_NO_PROTOTYPES
#include <vector>
#include <vulkan/vulkan.hpp>

#pragma once

namespace Vee::Vulkan {
class Shader;
}
namespace Vee::Vulkan {

class Pipeline final {
public:
    vk::PipelineLayout layout;
    vk::Pipeline pipeline;
};

class PipelineBuilder final {
public:
    Pipeline build(vk::Device device);
    PipelineBuilder& with_cache(vk::PipelineCache cache);
    PipelineBuilder& with_shader(const Shader& shader);

private:
    vk::PipelineCache m_cache;

    std::vector<vk::PipelineShaderStageCreateInfo> pipeline_shader_stage_infos;
};

} // namespace Vee::Vulkan