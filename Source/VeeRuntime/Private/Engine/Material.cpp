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


#include "Engine/Material.hpp"

#include "Engine/Texture.hpp"
#include "IApplication.hpp"
#include "MakeSharedEnabler.hpp"
#include "Platform/Filesystem.hpp"
#include "Renderer.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/RenderCtx.hpp"
#include "Renderer/Shader.hpp"

#include <entt/locator/locator.hpp>
#include <vector>


std::expected<std::shared_ptr<vee::Material>, vee::Material::CreateError> vee::Material::create(const std::shared_ptr<Texture>& texture) {
    std::shared_ptr<Material> material = std::make_shared<MakeSharedEnabler<Material>>();
    material->texture_ = texture;

    RenderCtx& ctx = entt::locator<IApplication>::value().get_renderer().get_ctx();

    // pipelines
    std::vector<std::byte> entity_vertex_shader_code = vee::platform::filesystem::read_binary_file("Resources/entity.vert.spv");
    std::vector<std::byte> tex_frag_shader_code = vee::platform::filesystem::read_binary_file("Resources/texture.frag.spv");

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


    // Set up image sampler descriptor
    vk::DescriptorSetAllocateInfo allocate_info = {ctx.descriptor_pool, material->pipeline_.descriptor_set_layout};
    std::vector<vk::DescriptorSet> sets = ctx.device.allocateDescriptorSets(allocate_info).value;
    material->descriptor_set_ = sets[0];

    vk::DescriptorImageInfo image_info = {tex_sampler, material->texture_->image_->view, vk::ImageLayout::eShaderReadOnlyOptimal};

    vk::WriteDescriptorSet descriptor_write = {material->descriptor_set_, 0, 0, vk::DescriptorType::eCombinedImageSampler, image_info, {}, {}};
    ctx.device.updateDescriptorSets(descriptor_write, {});

    return material;
}