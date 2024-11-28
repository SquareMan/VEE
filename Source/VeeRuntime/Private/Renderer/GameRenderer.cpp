//
// Created by Square on 10/27/2024.
//

#include "Renderer/GameRenderer.hpp"

#include "../../../VeeEditor/Public/EditorApplication.hpp"
#include "Engine/Entity.h"
#include "Engine/World.h"
#include "Platform/filesystem.hpp"
#include "Platform/Window.hpp"
#include "Renderer/Buffer.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/Pipeline.hpp"
#include "Renderer/RenderCtx.hpp"
#include "Renderer/Shader.hpp"
#include "Transform.h"
#include "Vertex.hpp"

#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <numbers>

#include <Components/CameraComponent.hpp>
#include <Components/SpriteRendererComponent.hpp>
#include <imgui.h>


namespace vee {
void GameRenderer::on_init() {
    auto& ctx = RenderCtx::GetService();

    // vertex and staging buffers
    float a = 4.0f * std::numbers::pi_v<float> / 3.0f;
    float b = 2.0f * std::numbers::pi_v<float> / 3.0f;
    float c = 0;
    const vee::Vertex vertices[] = {
        {{std::cos(a), std::sin(a)}, {1.0f, 0.0f, 0.0f}, {std::cos(a), std::sin(a)}},
        {{std::cos(c), std::sin(c)}, {0, 1.0f, 0.0f}, {std::cos(c), std::sin(c)}},
        {{std::cos(b), std::sin(b)}, {0.0f, 0.0f, 1.0f}, {std::cos(b), std::sin(b)}},
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    };
    const uint16_t indices[] = {0, 1, 2, 3, 4, 5, 6};
    {
        // 1MB staging buffer
        vk::BufferCreateInfo buffer_info({}, 1024 * 1024, vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive);
        auto [buf, alloc] =
            ctx.allocator
                .createBuffer(buffer_info, {vma::AllocationCreateFlagBits::eHostAccessSequentialWrite, vma::MemoryUsage::eAuto})
                .value;
        new (&staging_buffer_) vee::Buffer(buf, alloc, ctx.allocator);

        std::ignore = ctx.allocator.copyMemoryToAllocation(vertices, staging_buffer_.allocation, 0, sizeof(vertices));
        std::ignore = ctx.allocator.copyMemoryToAllocation(indices, staging_buffer_.allocation, sizeof(vertices), sizeof(indices));
    }

    {
        vk::BufferCreateInfo buffer_info = {
            {}, sizeof(vertices) + sizeof(indices), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::SharingMode::eExclusive
        };
        {
            auto [buf, alloc] =
                ctx.allocator
                    .createBuffer(buffer_info, {vma::AllocationCreateFlagBits::eDedicatedMemory, vma::MemoryUsage::eAuto})
                    .value;
            new (&vertex_buffer_) vee::Buffer(buf, alloc, ctx.allocator);
        }

        buffer_info.setUsage(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer);
        {
            auto [buf, alloc] =
                ctx.allocator
                    .createBuffer(buffer_info, {vma::AllocationCreateFlagBits::eDedicatedMemory, vma::MemoryUsage::eGpuOnly})
                    .value;
            new (&index_buffer_) vee::Buffer(buf, alloc, ctx.allocator);
        }
    }

    // Copy from staging to final buffers
    ctx.immediate_submit([&](vk::CommandBuffer cmd) {
        cmd.copyBuffer(staging_buffer_.buffer, vertex_buffer_.buffer, {{0, 0, sizeof(vertices)}});
        cmd.copyBuffer(staging_buffer_.buffer, index_buffer_.buffer, {{sizeof(vertices), 0, sizeof(indices)}});
    });


    // images/textures
    auto [width, height] = ctx.window->get_size();
    new (&game_image_) vee::Image(
        ctx.device, ctx.allocator, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc, {width, height, 1}, vk::Format::eB8G8R8A8Srgb, vk::ImageAspectFlagBits::eColor
    );

    tex_image_ = std::make_shared<vee::Image>(
        ctx.device, ctx.allocator, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, vk::Extent3D{16, 16, 1}, vk::Format::eB8G8R8A8Unorm, vk::ImageAspectFlagBits::eColor
    );

    {
        // checkerboard image
        const uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 1));
        const uint32_t magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
        std::array<uint32_t, 16 * 16> pixels; // for 16x16 checkerboard texture
        for (int x = 0; x < 16; x++) {
            for (int y = 0; y < 16; y++) {
                pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
            }
        }

        std::ignore = ctx.allocator.copyMemoryToAllocation(pixels.data(), staging_buffer_.allocation, 0, pixels.size() * sizeof(uint32_t));

        ctx.immediate_submit([&](vk::CommandBuffer cmd) {
            vee::vulkan::transition_image(cmd, tex_image_->image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

            vk::BufferImageCopy region = {{}, {}, {}, {vk::ImageAspectFlagBits::eColor, 0, 0, 1}, {}, {16, 16, 1}};
            cmd.copyBufferToImage(staging_buffer_.buffer, tex_image_->image, vk::ImageLayout::eTransferDstOptimal, region);

            vee::vulkan::transition_image(cmd, tex_image_->image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
        });

        entt::locator<std::shared_ptr<vee::Image>>::emplace(tex_image_);
    }

    // pipelines
    std::vector<char> entity_vertex_shader_code = vee::platform::filesystem::read_binary_file("Resources/entity.vert.spv"
    );
    std::vector<char> tex_frag_shader_code = vee::platform::filesystem::read_binary_file("Resources/texture.frag.spv"
    );

    vee::vulkan::Shader texture_fragment_shader = {ctx.device, vk::ShaderStageFlagBits::eFragment, tex_frag_shader_code};
    vee::vulkan::Shader entity_vertex_shader = {ctx.device, vk::ShaderStageFlagBits::eVertex, entity_vertex_shader_code};

    vk::Sampler tex_sampler = ctx.device.createSampler({}).value;
    vk::DescriptorSetLayoutBinding tex_binding = {0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, tex_sampler};
    // clang-format off
    sprite_pipeline_ = vee::vulkan::PipelineBuilder()
        .with_cache(ctx.pipeline_cache)
        .with_shader(texture_fragment_shader)
        .with_shader(entity_vertex_shader)
        .with_binding(tex_binding)
        .build(ctx.device);
    // clang-format on


    // descriptor pool
    {
        vk::DescriptorSetAllocateInfo allocate_info = {ctx.descriptor_pool, sprite_pipeline_.descriptor_set_layout};
        std::vector<vk::DescriptorSet> sets = ctx.device.allocateDescriptorSets(allocate_info).value;
        tex_descriptor_ = sets[0];

        vk::DescriptorImageInfo image_info = {tex_sampler, tex_image_->view, vk::ImageLayout::eShaderReadOnlyOptimal};

        vk::WriteDescriptorSet descriptor_write = {tex_descriptor_, 0, 0, vk::DescriptorType::eCombinedImageSampler, image_info, {}, {}};
        ctx.device.updateDescriptorSets(descriptor_write, {});
    }
}

void GameRenderer::on_render(vk::CommandBuffer cmd, uint32_t swapchain_idx) {
    auto& ctx = RenderCtx::GetService();

    auto time = static_cast<float>(glfwGetTime());
    Engine& engine = EditorApplication::GetService().get_engine();

    // TODO: deal with multiple cameras
    auto cams = engine.get_world().entt_registry.view<vee::CameraComponent, vee::Transform>();
    auto [e, cam, cam_transform] = *cams.each().begin();

    if (ImGui::Begin("Debug")) {
        ImGui::DragFloat2("Cam Pos", reinterpret_cast<float*>(&cam_transform.position));
        ImGui::DragFloat("Cam Rot", &cam_transform.rotation);
        ImGui::DragFloat2("Cam Scale", reinterpret_cast<float*>(&cam_transform.scale));
    }
    ImGui::End();

    const glm::mat4x4 proj = cam.calculate_view_projection(cam_transform);

    // Push view projection matrix
    // TODO: this should go in a buffer
    cmd.pushConstants(sprite_pipeline_.layout, vk::ShaderStageFlagBits::eVertex, sizeof(glm::mat4x4), sizeof(glm::mat4x4), &proj);

    vee::vulkan::transition_image(cmd, game_image_.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);
    {
        vk::ClearValue clear_value({0.3f, 0.77f, 0.5f, 1.0f});
        vk::RenderingAttachmentInfo render_attachment = {
            game_image_.view, vk::ImageLayout::eColorAttachmentOptimal, {}, {}, {}, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, clear_value
        };
        vk::RenderingInfo render_info = {{}, {{}, {game_image_.width(), game_image_.height()}}, 1, 0, render_attachment, {}, {}};

        cmd.beginRendering(render_info);
        {
            const vk::Rect2D scissor({{}, {game_image_.width(), game_image_.height()}});
            const vk::Viewport viewport(
                0, 0, static_cast<float>(game_image_.width()), static_cast<float>(game_image_.height()), 1.0f
            );

            cmd.setScissor(0, scissor);
            cmd.setViewport(0, viewport);

            auto view = engine.get_world().entt_registry.view<vee::Transform, vee::SpriteRendererComponent>(
            );
            for (const auto [ent, trans, spr] : view.each()) {
                cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, spr.pipeline_.pipeline);

                glm::mat4x4 local_to_world = trans.to_mat();
                cmd.pushConstants(spr.pipeline_.layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4x4), &local_to_world);
                cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, spr.pipeline_.layout, 0, spr.descriptor_set_, {});
                cmd.bindVertexBuffers(0, vertex_buffer_.buffer, {{0}});
                cmd.bindIndexBuffer(index_buffer_.buffer, 0, vk::IndexType::eUint16);
                cmd.drawIndexed(4, 1, 3, 0, 0);
            }

            cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, sprite_pipeline_.pipeline);
            vee::Transform({320.f, 320.f}, time, {100.f, 100.f});
            glm::mat4x4 triangle_transform = vee::Transform({320.f, 320.f}, time, {100.f, 100.f}).to_mat();
            cmd.pushConstants(sprite_pipeline_.layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4x4), &triangle_transform);
            cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, sprite_pipeline_.layout, 0, tex_descriptor_, {});
            cmd.bindVertexBuffers(0, vertex_buffer_.buffer, {{0}});
            cmd.bindIndexBuffer(index_buffer_.buffer, 0, vk::IndexType::eUint16);
            cmd.drawIndexed(3, 1, 0, 0, 0);
        }
        cmd.endRendering();
    }

    vee::vulkan::transition_image(cmd, game_image_.image, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eTransferSrcOptimal);

    // swapchain image transition
    vee::vulkan::transition_image(cmd, ctx.swapchain.images[swapchain_idx], vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

    {
        vk::ImageBlit2 region = {
            vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1},
            {vk::Offset3D(),
             vk::Offset3D{
                 static_cast<int32_t>(game_image_.width()), static_cast<int32_t>(game_image_.height()), 1
             }},
            vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1},
            {vk::Offset3D(),
             vk::Offset3D{
                 static_cast<int32_t>(ctx.swapchain.width), static_cast<int32_t>(ctx.swapchain.height), 1
             }},
        };
        vk::BlitImageInfo2 blit_info = {
            game_image_.image,
            vk::ImageLayout::eTransferSrcOptimal,
            ctx.swapchain.images[swapchain_idx],
            vk::ImageLayout::eTransferDstOptimal,
            region,
        };
        cmd.blitImage2(blit_info);
    }

    // swapchain image transition
    vee::vulkan::transition_image(cmd, ctx.swapchain.images[swapchain_idx], vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eColorAttachmentOptimal);
}
} // namespace vee