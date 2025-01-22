//
// Created by Square on 11/18/2024.
//

#include "Components/SpriteRendererComponent.hpp"

#include "Assert.hpp"
#include "Platform/filesystem.hpp"
#include "Renderer/RenderCtx.hpp"
#include "Renderer/Shader.hpp"

#include <entt/locator/locator.hpp>

vee::SpriteRendererComponent::SpriteRendererComponent(const Sprite& sprite)
    : sprite_(sprite) {
    auto& ctx = RenderCtx::GetService();

    // TODO: Reuse these resources when appropriate
    std::vector<char> entity_vertex_shader_code = vee::platform::filesystem::read_binary_file("Resources/entity.vert.spv"
    );
    std::vector<char> tex_frag_shader_code = vee::platform::filesystem::read_binary_file("Resources/texture.frag.spv"
    );

    vee::vulkan::Shader texture_fragment_shader = {ctx.device, vk::ShaderStageFlagBits::eFragment, tex_frag_shader_code};
    vee::vulkan::Shader entity_vertex_shader = {ctx.device, vk::ShaderStageFlagBits::eVertex, entity_vertex_shader_code};

    vk::Sampler tex_sampler = ctx.device.createSampler({}).value;
    vk::DescriptorSetLayoutBinding tex_binding = {0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, tex_sampler};

    pipeline_ =
        vee::vulkan::PipelineBuilder()
            .with_cache(ctx.pipeline_cache)
            .with_binding(tex_binding)
            .with_shader(entity_vertex_shader)
            .with_shader(texture_fragment_shader)
            .build(ctx.device);


    vk::DescriptorSetAllocateInfo allocate_info = {ctx.descriptor_pool, pipeline_.descriptor_set_layout};
    std::vector<vk::DescriptorSet> sets = ctx.device.allocateDescriptorSets(allocate_info).value;
    descriptor_set_ = sets[0];

    VASSERT(sprite.image_ != nullptr);
    vk::DescriptorImageInfo image_info = {tex_sampler, sprite_.image_->view, vk::ImageLayout::eShaderReadOnlyOptimal};

    vk::WriteDescriptorSet descriptor_write = {descriptor_set_, 0, 0, vk::DescriptorType::eCombinedImageSampler, image_info, {}, {}};
    ctx.device.updateDescriptorSets(descriptor_write, {});
}

vee::SpriteRendererComponent::SpriteRendererComponent(Sprite&& sprite)
    : sprite_(sprite) {}