//
// Created by Square on 10/3/2024.
//

#define VK_NO_PROTOTYPES
#include <vector>
#include <vulkan/vulkan_core.h>

#pragma once

namespace Vee::Vulkan {
class Shader;
}
namespace Vee::Vulkan {

class Pipeline final {
public:
    VkPipelineLayout layout;
    VkPipeline pipeline;
};

class PipelineBuilder final {
public:
    Pipeline build(VkDevice device);
    PipelineBuilder& with_cache(VkPipelineCache cache);
    PipelineBuilder& with_renderpass(VkRenderPass renderpass);
    PipelineBuilder& with_shader(const Shader& shader);

private:
    VkPipelineCache m_cache = VK_NULL_HANDLE;
    VkRenderPass m_renderpass = VK_NULL_HANDLE;

    std::vector<VkPipelineShaderStageCreateInfo> pipeline_shader_stage_infos;
};

} // namespace Vee::Vulkan