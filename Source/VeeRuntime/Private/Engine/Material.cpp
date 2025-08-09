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

#include "Assert.hpp"
#include "Engine/Texture.hpp"
#include "IApplication.hpp"
#include "JobManager.hpp"
#include "MakeSharedEnabler.hpp"
#include "Platform/Filesystem.hpp"
#include "Renderer.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/RenderCtx.hpp"
#include "Renderer/Shader.hpp"
#include "tracy/Tracy.hpp"

#include <entt/locator/locator.hpp>
#include <shaderc/shaderc.hpp>
#include <vector>

// FIXME: Do not include this in shipping builds. Prebuild spirv
class Compiler {
    shaderc::Compiler compiler_;

public:
    std::expected<shaderc::SpvCompilationResult, std::string> compile(const char* path) {
        ZoneScoped;
        shaderc::CompileOptions compile_options;
        std::vector<std::byte> source = vee::platform::filesystem::read_binary_file(path);
        shaderc::SpvCompilationResult result = compiler_.CompileGlslToSpv(
            reinterpret_cast<const char*>(source.data()), source.size(), shaderc_glsl_infer_from_source, path, "main", compile_options
        );

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            vee::log_error("Shader Compilation for \"{}\" Failed!\n{}", path, result.GetErrorMessage());
            return std::unexpected(result.GetErrorMessage());
        }
        return result;
    }
};

static Compiler g_compiler;

std::expected<std::shared_ptr<vee::Material>, vee::Material::CreateError> vee::Material::create(const std::shared_ptr<Texture>& texture) {
    std::shared_ptr<Material> material = std::make_shared<MakeSharedEnabler<Material>>();
    material->texture_ = texture;

    RenderCtx& ctx = entt::locator<IApplication>::value().get_renderer().get_ctx();

    // pipelines
    auto entity_vertex_shader_code = g_compiler.compile(VEE_ENGINE_RESOURCES_PATH "/entity.vert");
    if (!entity_vertex_shader_code) {
        log_error("Failed to compile material vertex shader");
        return std::unexpected(CreateError{});
    }
    auto tex_frag_shader_code = g_compiler.compile(VEE_ENGINE_RESOURCES_PATH "/texture.frag");
    if(!tex_frag_shader_code) {
        log_error("Failed to compile material fragment shader");
        return std::unexpected(CreateError{});
    }


    vulkan::Shader entity_vertex_shader = {ctx.device, vk::ShaderStageFlagBits::eVertex, entity_vertex_shader_code.value()};
    vulkan::Shader texture_fragment_shader = {ctx.device, vk::ShaderStageFlagBits::eFragment, tex_frag_shader_code.value()};

    vk::Sampler tex_sampler = ctx.device.createSampler({}).value;
    vk::DescriptorSetLayoutBinding tex_binding = {0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, tex_sampler};
    // clang-format off
    material->pipeline_ = vulkan::PipelineBuilder()
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