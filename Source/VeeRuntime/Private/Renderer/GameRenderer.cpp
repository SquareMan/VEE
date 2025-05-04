//
// Created by Square on 10/27/2024.
//

#include "Renderer/GameRenderer.hpp"

#include "Application.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Entity.h"
#include "Engine/World.h"
#include "Platform/Filesystem.hpp"
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
#ifdef VEE_WITH_EDITOR
#include <imgui.h>
#endif
#include "Engine/Material.hpp"
#include "Engine/Texture.hpp"


#include <stb_image.h>


namespace vee {
void GameRenderer::on_init(RenderCtx& ctx) {
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
        std::ignore = ctx.allocator.copyMemoryToAllocation(vertices, ctx.staging_buffer.allocation, 0, sizeof(vertices));
        std::ignore = ctx.allocator.copyMemoryToAllocation(indices, ctx.staging_buffer.allocation, sizeof(vertices), sizeof(indices));
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
        cmd.copyBuffer(ctx.staging_buffer.buffer, vertex_buffer_.buffer, {{0, 0, sizeof(vertices)}});
        cmd.copyBuffer(ctx.staging_buffer.buffer, index_buffer_.buffer, {{sizeof(vertices), 0, sizeof(indices)}});
    });


    // images/textures
    auto [width, height] = ctx.window->get_size();
    new (&game_image_) vee::Image(
        ctx.device, ctx.allocator, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc, {width, height, 1}, vk::Format::eB8G8R8A8Srgb, vk::ImageAspectFlagBits::eColor
    );
}

void GameRenderer::on_render(vk::CommandBuffer cmd, uint32_t swapchain_idx) {
    auto& ctx = entt::locator<IApplication>::value().get_renderer().get_ctx();

    auto time = static_cast<float>(glfwGetTime());
    Engine& engine = entt::locator<IApplication>::value().get_engine();

    // TODO: deal with multiple cameras
    auto cams = engine.get_world().entt_registry.view<vee::CameraComponent, vee::Transform>();
    auto [e, cam, cam_transform] = *cams.each().begin();

#ifdef VEE_WITH_EDITOR
    if (ImGui::Begin("Debug")) {
        ImGui::DragFloat2("Cam Pos", reinterpret_cast<float*>(&cam_transform.position));
        ImGui::DragFloat("Cam Rot", &cam_transform.rotation);
        ImGui::DragFloat2("Cam Scale", reinterpret_cast<float*>(&cam_transform.scale));
    }
    ImGui::End();
#endif

    const glm::mat4x4 proj = cam.calculate_view_projection(cam_transform);

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
                std::shared_ptr<Material>& mat = spr.sprite_.material_;

                // Push view projection matrix
                // TODO: this should go in a buffer
                cmd.pushConstants(mat->pipeline_.layout, vk::ShaderStageFlagBits::eVertex, sizeof(glm::mat4x4), sizeof(glm::mat4x4), &proj);

                cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, mat->pipeline_.pipeline);

                glm::mat4x4 local_to_world = trans.to_mat();
                cmd.pushConstants(mat->pipeline_.layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4x4), &local_to_world);
                cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mat->pipeline_.layout, 0, mat->descriptor_set_, {});
                cmd.bindVertexBuffers(0, vertex_buffer_.buffer, {{0}});
                cmd.bindIndexBuffer(index_buffer_.buffer, 0, vk::IndexType::eUint16);
                cmd.drawIndexed(4, 1, 3, 0, 0);
            }
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