//
// Created by Square on 10/3/2024.
//

#include "Pipeline.hpp"

#include "Shader.hpp"
#include "VkUtil.hpp"

#include <iostream>
#include <volk/volk.h>
#include <vulkan/vk_enum_string_helper.h>

namespace Vee {
Vulkan::Pipeline Vulkan::PipelineBuilder::build(VkDevice device) {
    // build layout
    // TODO: Configurable layouts
    const VkPushConstantRange push_constants[]{{
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset = 0,
        .size = 4,
    }};
    VkPipelineLayoutCreateInfo layout_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = push_constants,
    };
    VkPipelineLayout layout = VK_NULL_HANDLE;
    VK_CHECK(vkCreatePipelineLayout(device, &layout_info, nullptr, &layout));

    VkPipelineColorBlendAttachmentState color_blend_attachment{
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
                          | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo color_blend_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachment,
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    };

    VkPipelineRasterizationStateCreateInfo rasterization_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .lineWidth = 1.0f,
    };

    VkRect2D scissor{};
    VkViewport viewport{};

    VkPipelineViewportStateCreateInfo viewport_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };

    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(std::size(dynamic_states)),
        .pDynamicStates = dynamic_states,
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
    };

    VkPipelineMultisampleStateCreateInfo multisample_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    };


    VkGraphicsPipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = static_cast<uint32_t>(pipeline_shader_stage_infos.size()),
        .pStages = pipeline_shader_stage_infos.data(),
        .pVertexInputState = &vertex_input_state_info,
        .pInputAssemblyState = &input_assembly_state_info,
        .pViewportState = &viewport_state_info,
        .pRasterizationState = &rasterization_state_info,
        .pMultisampleState = &multisample_state_info,
        .pColorBlendState = &color_blend_state_info,
        .pDynamicState = &dynamic_state_info,
        .layout = layout,
        .renderPass = m_renderpass,
    };

    VkPipeline pipeline;
    VK_CHECK(vkCreateGraphicsPipelines(device, m_cache, 1, &pipeline_info, nullptr, &pipeline));
    return Pipeline{layout, pipeline};
}

Vulkan::PipelineBuilder& Vulkan::PipelineBuilder::with_cache(VkPipelineCache cache) {
    m_cache = cache;
    return *this;
}
Vulkan::PipelineBuilder& Vulkan::PipelineBuilder::with_renderpass(VkRenderPass renderpass) {
    m_renderpass = renderpass;
    return *this;
}
Vulkan::PipelineBuilder& Vulkan::PipelineBuilder::with_shader(const Shader& shader) {
    const VkPipelineShaderStageCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = static_cast<VkShaderStageFlagBits>(shader.m_stage),
        .module = shader.m_module,
        .pName = shader.entrypoint()
    };
    pipeline_shader_stage_infos.push_back(info);

    return *this;
}
} // namespace Vee