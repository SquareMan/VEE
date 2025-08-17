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
#include "Renderer.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/RenderCtx.hpp"
#include "Renderer/Shader.hpp"
#include "tracy/Tracy.hpp"

#include <entt/locator/locator.hpp>
#include <slang/slang-com-ptr.h>
#include <slang/slang.h>
#include <vector>

#define STRINGIFY(x) #x

// FIXME: Do not include this in shipping builds. Prebuild spirv
class Compiler {
    Slang::ComPtr<slang::IGlobalSession> globalSession_;

public:
    Compiler() {
        ZoneScoped;
        slang::createGlobalSession(globalSession_.writeRef());
    }

    std::expected<std::vector<uint32_t>, std::string> compile(const char* path) const {
        ZoneScoped;
        using namespace slang;

        vee::log_trace("Starting Shader Compile: {}", path);

        TargetDesc target_desc;
        target_desc.format = SLANG_SPIRV;
        target_desc.profile = globalSession_->findProfile("spirv_1_6");

        // FIXME: This should be controlled with a runtime engine configuration option (for
        // non-shipping builds)
#ifdef VEE_DEBUG
        CompilerOptionEntry debug{CompilerOptionName::DebugInformation, {.intValue0 = SLANG_DEBUG_INFO_LEVEL_MAXIMAL}};
        target_desc.compilerOptionEntries = &debug;
        target_desc.compilerOptionEntryCount = 1;
#endif

        SessionDesc session_desc;
        session_desc.targets = &target_desc;
        session_desc.targetCount = 1;
        session_desc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;

        auto preprocessor_macro_defs = std::to_array<PreprocessorMacroDesc>({{"VEE_DEBUG", STRINGIFY(VEE_DEBUG)}
        });
        session_desc.preprocessorMacros = preprocessor_macro_defs.data();
        session_desc.preprocessorMacroCount = preprocessor_macro_defs.size();

        Slang::ComPtr<ISession> session;
        globalSession_->createSession(session_desc, session.writeRef());

        Slang::ComPtr<IModule> module;
        Slang::ComPtr<IBlob> module_diagnostics;
        module = session->loadModule(path, module_diagnostics.writeRef());

        if (module_diagnostics != nullptr) {
            vee::log(
                !module ? vee::LogSeverity::Error : vee::LogSeverity::Warning,
                "Failed to load module\n{}",
                std::string_view{
                    static_cast<const char*>(module_diagnostics->getBufferPointer()),
                    module_diagnostics->getBufferSize()
                }
            );
        }
        if (!module) {
            return std::unexpected("");
        }

        Slang::ComPtr<IComponentType> component;
        Slang::ComPtr<IBlob> link_diagnostics;
        SlangResult link_result = module->link(component.writeRef(), link_diagnostics.writeRef());

        if (link_diagnostics != nullptr) {
            vee::log(
                SLANG_FAILED(link_result) ? vee::LogSeverity::Error : vee::LogSeverity::Warning,
                "Failed to link component\n{}",
                std::string_view{
                    static_cast<const char*>(link_diagnostics->getBufferPointer()), link_diagnostics->getBufferSize()
                }
            );
        }
        if (SLANG_FAILED(link_result)) {
            return std::unexpected("");
        }

        Slang::ComPtr<IBlob> spirv_blob;
        Slang::ComPtr<IBlob> code_diagnostics;
        SlangResult code_result =
            component->getTargetCode(0, spirv_blob.writeRef(), code_diagnostics.writeRef());
        if (code_diagnostics != nullptr) {
            vee::log(
                SLANG_FAILED(code_result) ? vee::LogSeverity::Error : vee::LogSeverity::Warning,
                "{}: Failed to get target code\n{}",
                path,
                std::string_view{
                    static_cast<const char*>(code_diagnostics->getBufferPointer()), code_diagnostics->getBufferSize()
                }
            );
        }
        if (SLANG_FAILED(code_result)) {
            return std::unexpected("");
        }

        std::size_t code_size = spirv_blob->getBufferSize();
        const void* code = spirv_blob->getBufferPointer();
        VASSERT(code_size % 4 == 0);

        vee::log_info("Shader Compiled: {}", path);
        return std::vector(static_cast<const uint32_t*>(code), static_cast<const uint32_t*>(code) + code_size / 4);
    }
};

static Compiler g_compiler;

std::expected<std::shared_ptr<vee::Material>, vee::Material::CreateError> vee::Material::create(const std::shared_ptr<Texture>& texture) {
    std::shared_ptr<Material> material = std::make_shared<MakeSharedEnabler<Material>>();

    RenderCtx& ctx = entt::locator<IApplication>::value().get_renderer().get_ctx();

    // pipelines
    auto sprite_shader_code = g_compiler.compile(VEE_ENGINE_RESOURCES_PATH "/sprite.slang");
    if (!sprite_shader_code) {
        log_error("Failed to compile material vertex shader");
        return std::unexpected(CreateError{});
    }


    // FIXME: These could share the same vk::ShaderModule but right now they construct it themselves
    vulkan::Shader entity_vertex_shader = {
        ctx.device, vk::ShaderStageFlagBits::eVertex, sprite_shader_code.value(), "vertexMain"
    };
    vulkan::Shader texture_fragment_shader = {
        ctx.device, vk::ShaderStageFlagBits::eFragment, sprite_shader_code.value(), "fragmentMain"
    };

    material->tex_sampler_ = ctx.device.createSampler({}).value;
    vk::DescriptorSetLayoutBinding tex_binding = {0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, material->tex_sampler_};
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

    material->set_texture(texture);

    return material;
}
void vee::Material::set_texture(const std::shared_ptr<Texture>& texture) {
    texture_ = texture;

    RenderCtx& ctx = entt::locator<IApplication>::value().get_renderer().get_ctx();
    vk::DescriptorImageInfo image_info = {tex_sampler_, texture_->image_->view, vk::ImageLayout::eShaderReadOnlyOptimal};

    vk::WriteDescriptorSet descriptor_write = {descriptor_set_, 0, 0, vk::DescriptorType::eCombinedImageSampler, image_info, {}, {}};
    ctx.device.updateDescriptorSets(descriptor_write, {});
}

std::shared_ptr<vee::Texture>& vee::Material::get_texture() {
    return texture_;
}