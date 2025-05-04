//
// Created by Square on 5/4/2025.
//

#include "Engine/Material.hpp"

#include "Engine/Texture.hpp"
#include "entt/locator/locator.hpp"
#include "IApplication.hpp"
#include "Platform/Filesystem.hpp"
#include "Renderer.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/RenderCtx.hpp"
#include "Renderer/Shader.hpp"

#include <vector>


std::expected<std::shared_ptr<vee::Material>, vee::Material::CreateError> vee::Material::create(const std::shared_ptr<Texture>& texture) {
    struct MakeSharedEnabler : Material {
        MakeSharedEnabler()
            : Material() {}
    };
    std::shared_ptr<Material> material = std::make_shared<MakeSharedEnabler>();
    material->texture_ = texture;

    RenderCtx& ctx = entt::locator<IApplication>::value().get_renderer().get_ctx();

    // pipelines
    std::vector<char> entity_vertex_shader_code = vee::platform::filesystem::read_binary_file("Resources/entity.vert.spv");
    std::vector<char> tex_frag_shader_code = vee::platform::filesystem::read_binary_file("Resources/texture.frag.spv");

    vee::vulkan::Shader texture_fragment_shader = {ctx.device, vk::ShaderStageFlagBits::eFragment, tex_frag_shader_code};
    vee::vulkan::Shader entity_vertex_shader = {ctx.device, vk::ShaderStageFlagBits::eVertex, entity_vertex_shader_code};

    vk::Sampler tex_sampler = ctx.device.createSampler({}).value;
    vk::DescriptorSetLayoutBinding tex_binding = {0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, tex_sampler};
    // clang-format off
    material->pipeline_ = vee::vulkan::PipelineBuilder()
        .with_cache(ctx.pipeline_cache)
        .with_shader(texture_fragment_shader)
        .with_shader(entity_vertex_shader)
        .with_binding(tex_binding)
        .build(ctx.device);
    // clang-format on


    // descriptor pool
    {
        vk::DescriptorSetAllocateInfo allocate_info = {ctx.descriptor_pool, material->pipeline_.descriptor_set_layout};
        std::vector<vk::DescriptorSet> sets = ctx.device.allocateDescriptorSets(allocate_info).value;
        material->descriptor_set_ = sets[0];

        vk::DescriptorImageInfo image_info = {tex_sampler, material->texture_->image_->view, vk::ImageLayout::eShaderReadOnlyOptimal};

        vk::WriteDescriptorSet descriptor_write = {material->descriptor_set_, 0, 0, vk::DescriptorType::eCombinedImageSampler, image_info, {}, {}};
        ctx.device.updateDescriptorSets(descriptor_write, {});
    }

    return material;
}