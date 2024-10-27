//
// Created by Square on 10/3/2024.
//

#include "Renderer/Pipeline.hpp"

#include "Renderer/Shader.hpp"
#include "Vertex.hpp"
#include "Renderer/VkUtil.hpp"

namespace vee {
vulkan::Pipeline vulkan::PipelineBuilder::build(vk::Device device) {
    // build layout
    // TODO: Configurable layouts
    const vk::PushConstantRange push_constants[]{{
        vk::ShaderStageFlagBits::eVertex,
        0,
        4,
    }};

    vk::DescriptorSetLayoutCreateInfo set_layout_info = {{}, descriptor_set_layout_bindings};
    vk::DescriptorSetLayout descriptor_layout = device.createDescriptorSetLayout(set_layout_info);

    vk::PipelineLayoutCreateInfo layout_info({}, descriptor_layout, push_constants);
    VkPipelineLayout layout = device.createPipelineLayout(layout_info);

    vk::PipelineColorBlendAttachmentState color_blend_attachment;
    color_blend_attachment.setColorWriteMask(
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
        | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
    );

    vk::PipelineColorBlendStateCreateInfo color_blend_state_info;
    color_blend_state_info.setAttachments(color_blend_attachment);

    auto binding_description = Vertex::binding_description();
    auto attribute_descriptions = Vertex::attribute_descriptions();
    vk::PipelineVertexInputStateCreateInfo vertex_input_state_info(
        {}, binding_description, attribute_descriptions
    );

    vk::PipelineRasterizationStateCreateInfo rasterization_state_info(
        {},
        {},
        {},
        vk::PolygonMode::eFill,
        vk::CullModeFlagBits::eBack,
        vk::FrontFace::eClockwise,
        {},
        {},
        {},
        {},
        1.0f
    );

    vk::PipelineViewportStateCreateInfo viewport_state_info = {};
    viewport_state_info.viewportCount = 1;
    viewport_state_info.scissorCount = 1;

    vk::DynamicState dynamic_states[] = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };
    vk::PipelineDynamicStateCreateInfo dynamic_state_info({}, dynamic_states);

    vk::PipelineInputAssemblyStateCreateInfo input_assembly_state_info(
        {}, vk::PrimitiveTopology::eTriangleFan
    );
    vk::PipelineMultisampleStateCreateInfo multisample_state_info({}, vk::SampleCountFlagBits::e1);

    vk::Format format = vk::Format::eB8G8R8A8Srgb;
    vk::PipelineRenderingCreateInfo rendering_info = {{}, format};

    vk::GraphicsPipelineCreateInfo pipeline_info(
        {},
        pipeline_shader_stage_infos,
        &vertex_input_state_info,
        &input_assembly_state_info,
        {},
        &viewport_state_info,
        &rasterization_state_info,
        &multisample_state_info,
        {},
        &color_blend_state_info,
        &dynamic_state_info,
        layout,
        {},
        {},
        {},
        {},
        &rendering_info
    );

    vk::Pipeline pipeline = device.createGraphicsPipeline(m_cache, pipeline_info).value;
    return Pipeline{layout, pipeline, descriptor_layout};
}

vulkan::PipelineBuilder& vulkan::PipelineBuilder::with_cache(vk::PipelineCache cache) {
    m_cache = cache;
    return *this;
}
vulkan::PipelineBuilder& vulkan::PipelineBuilder::with_shader(const Shader& shader) {
    vk::PipelineShaderStageCreateInfo info({}, shader.m_stage, shader.m_module, shader.entrypoint());
    pipeline_shader_stage_infos.push_back(info);

    return *this;
}

vulkan::PipelineBuilder&
vulkan::PipelineBuilder::with_binding(const vk::DescriptorSetLayoutBinding& binding) {
    descriptor_set_layout_bindings.push_back(binding);

    return *this;
}
} // namespace vee