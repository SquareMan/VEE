//
// Created by Square on 10/27/2024.
//

#include "GameRenderer.hpp"

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

#include <imgui.h>


namespace ht {
void GameRenderer::on_init(std::shared_ptr<vee::RenderCtx>& ctx) {
    ctx_ = ctx;

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
        vk::BufferCreateInfo buffer_info(
            {}, 1024 * 1024, vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive
        );
        auto [buf, alloc] = ctx_->allocator
                                .createBuffer(
                                    buffer_info,
                                    {vma::AllocationCreateFlagBits::eHostAccessSequentialWrite,
                                     vma::MemoryUsage::eAuto}
                                )
                                .value;
        new (&staging_buffer_) vee::Buffer(buf, alloc, ctx_->allocator);

        std::ignore = ctx_->allocator.copyMemoryToAllocation(
            vertices, staging_buffer_.allocation, 0, sizeof(vertices)
        );
        std::ignore = ctx_->allocator.copyMemoryToAllocation(
            indices, staging_buffer_.allocation, sizeof(vertices), sizeof(indices)
        );
    }

    {
        vk::BufferCreateInfo buffer_info = {
            {},
            sizeof(vertices) + sizeof(indices),
            vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
            vk::SharingMode::eExclusive
        };
        {
            auto [buf, alloc] =
                ctx_->allocator
                    .createBuffer(
                        buffer_info,
                        {vma::AllocationCreateFlagBits::eDedicatedMemory, vma::MemoryUsage::eAuto}
                    )
                    .value;
            new (&vertex_buffer_) vee::Buffer(buf, alloc, ctx_->allocator);
        }

        buffer_info.setUsage(
            vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer
        );
        {
            auto [buf, alloc] = ctx_->allocator
                                    .createBuffer(
                                        buffer_info,
                                        {vma::AllocationCreateFlagBits::eDedicatedMemory,
                                         vma::MemoryUsage::eGpuOnly}
                                    )
                                    .value;
            new (&index_buffer_) vee::Buffer(buf, alloc, ctx_->allocator);
        }
    }

    // Copy from staging to final buffers
    ctx_->immediate_submit([&](vk::CommandBuffer cmd) {
        cmd.copyBuffer(staging_buffer_.buffer, vertex_buffer_.buffer, {{0, 0, sizeof(vertices)}});
        cmd.copyBuffer(
            staging_buffer_.buffer, index_buffer_.buffer, {{sizeof(vertices), 0, sizeof(indices)}}
        );
    });


    // images/textures
    auto [width, height] = ctx_->window->get_size();
    new (&game_image_) vee::Image(
        ctx_->device,
        ctx_->allocator,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
        {width, height, 1},
        vk::Format::eB8G8R8A8Srgb,
        vk::ImageAspectFlagBits::eColor
    );

    new (&tex_image_) vee::Image(
        ctx_->device,
        ctx_->allocator,
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
        {16, 16, 1},
        vk::Format::eB8G8R8A8Unorm,
        vk::ImageAspectFlagBits::eColor
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

        std::ignore = ctx_->allocator.copyMemoryToAllocation(
            pixels.data(), staging_buffer_.allocation, 0, pixels.size() * sizeof(uint32_t)
        );

        ctx_->immediate_submit([&](vk::CommandBuffer cmd) {
            vee::vulkan::transition_image(
                cmd,
                tex_image_.image,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eTransferDstOptimal
            );

            vk::BufferImageCopy region = {
                {}, {}, {}, {vk::ImageAspectFlagBits::eColor, 0, 0, 1}, {}, {16, 16, 1}
            };
            cmd.copyBufferToImage(
                staging_buffer_.buffer,
                tex_image_.image,
                vk::ImageLayout::eTransferDstOptimal,
                region
            );

            vee::vulkan::transition_image(
                cmd,
                tex_image_.image,
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal
            );
        });
    }

    // pipelines
    vk::PipelineCache pipeline_cache = ctx_->device.createPipelineCache({}).value;

    std::vector<char> entity_vertex_shader_code =
        vee::platform::filesystem::read_binary_file("Resources/entity.vert.spv");
    std::vector<char> vert_color_frag_shader_code =
        vee::platform::filesystem::read_binary_file("Resources/color.frag.spv");
    std::vector<char> tex_frag_shader_code =
        vee::platform::filesystem::read_binary_file("Resources/texture.frag.spv");

    vee::vulkan::Shader vert_color_fragment_shader = {
        ctx_->device, vk::ShaderStageFlagBits::eFragment, vert_color_frag_shader_code
    };
    vee::vulkan::Shader texture_fragment_shader = {
        ctx_->device, vk::ShaderStageFlagBits::eFragment, tex_frag_shader_code
    };
    vee::vulkan::Shader entity_vertex_shader = {
        ctx_->device, vk::ShaderStageFlagBits::eVertex, entity_vertex_shader_code
    };

    vk::Sampler tex_sampler = ctx_->device.createSampler({}).value;
    vk::DescriptorSetLayoutBinding tex_binding = {
        0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, tex_sampler
    };
    // clang-format off
    triangle_pipeline_ = vee::vulkan::PipelineBuilder()
        .with_cache(pipeline_cache)
        .with_shader(vert_color_fragment_shader)
        .with_shader(entity_vertex_shader)
        .build(ctx_->device);

    square_pipeline_ = vee::vulkan::PipelineBuilder()
        .with_cache(pipeline_cache)
        .with_shader(texture_fragment_shader)
        .with_shader(entity_vertex_shader)
        .with_binding(tex_binding)
        .build(ctx_->device);
    // clang-format on


    // descriptor pool
    {
        vk::DescriptorPoolSize pool_sizes[] = {{vk::DescriptorType::eCombinedImageSampler, 1}};
        descriptor_pool_ =
            ctx_->device
                .createDescriptorPool(
                    {vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 1, pool_sizes}
                )
                .value;

        vk::DescriptorSetAllocateInfo allocate_info = {
            descriptor_pool_, square_pipeline_.descriptor_set_layout
        };
        std::vector<vk::DescriptorSet> sets =
            ctx_->device.allocateDescriptorSets(allocate_info).value;
        tex_descriptor_ = sets[0];

        vk::DescriptorImageInfo image_info = {
            tex_sampler, tex_image_.view, vk::ImageLayout::eShaderReadOnlyOptimal
        };

        vk::WriteDescriptorSet descriptor_write = {
            tex_descriptor_, 0, 0, vk::DescriptorType::eCombinedImageSampler, image_info, {}, {}
        };
        ctx_->device.updateDescriptorSets(descriptor_write, {});
    }
}

void GameRenderer::on_render(vk::CommandBuffer cmd, uint32_t swapchain_idx) {
    auto time = static_cast<float>(glfwGetTime());

    static glm::vec2 cam_pos = {};
    static float cam_rot = 0;
    static glm::vec2 cam_scale = {1, 1};
    if (ImGui::Begin("Debug")) {
        ImGui::DragFloat2("Cam Pos", reinterpret_cast<float*>(&cam_pos));
        ImGui::DragFloat("Cam Rot", &cam_rot);
        ImGui::DragFloat2("Cam Scale", reinterpret_cast<float*>(&cam_scale));
    }
    ImGui::End();

    vee::Transform cam_transform(cam_pos, glm::radians(cam_rot), cam_scale);
    const glm::mat4x4 proj = glm::ortho<float>(
                                 -static_cast<float>(ctx_->swapchain.width) / 2,
                                 static_cast<float>(ctx_->swapchain.width) / 2,
                                 -static_cast<float>(ctx_->swapchain.height) / 2,
                                 static_cast<float>(ctx_->swapchain.height) / 2
                             )
                             * glm::inverse(cam_transform.to_mat());

    cmd.pushConstants(
        square_pipeline_.layout,
        vk::ShaderStageFlagBits::eVertex,
        sizeof(glm::mat4x4),
        sizeof(glm::mat4x4),
        &proj
    );

    vee::vulkan::transition_image(
        cmd, game_image_.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal
    );
    {
        vk::ClearValue clear_value({0.3f, 0.77f, 0.5f, 1.0f});
        vk::RenderingAttachmentInfo render_attachment = {
            game_image_.view,
            vk::ImageLayout::eColorAttachmentOptimal,
            {},
            {},
            {},
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            clear_value
        };
        vk::RenderingInfo render_info = {
            {}, {{}, {game_image_.width(), game_image_.height()}}, 1, 0, render_attachment, {}, {}
        };

        cmd.beginRendering(render_info);
        {
            const vk::Rect2D scissor({{}, {game_image_.width(), game_image_.height()}});
            const vk::Viewport viewport(
                0,
                0,
                static_cast<float>(game_image_.width()),
                static_cast<float>(game_image_.height()),
                1.0f
            );

            cmd.setScissor(0, scissor);
            cmd.setViewport(0, viewport);

            cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, square_pipeline_.pipeline);
            glm::mat4x4 square_transform =
                vee::Transform(
                    100.f
                        * glm::vec2{((time - static_cast<uint32_t>(time)) - 0.5) * 2, std::sin(time)},
                    0,
                    {100.f, 100.f}
                )
                    .to_mat();
            cmd.pushConstants(
                square_pipeline_.layout,
                vk::ShaderStageFlagBits::eVertex,
                0,
                sizeof(glm::mat4x4),
                &square_transform
            );
            cmd.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics, square_pipeline_.layout, 0, tex_descriptor_, {}
            );
            cmd.bindVertexBuffers(0, vertex_buffer_.buffer, {{0}});
            cmd.bindIndexBuffer(index_buffer_.buffer, 0, vk::IndexType::eUint16);
            cmd.drawIndexed(4, 1, 3, 0, 0);

            cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, triangle_pipeline_.pipeline);
            vee::Transform({320.f, 320.f}, time, {100.f, 100.f});
            glm::mat4x4 triangle_transform =
                vee::Transform({320.f, 320.f}, time, {100.f, 100.f}).to_mat();
            cmd.pushConstants(
                triangle_pipeline_.layout,
                vk::ShaderStageFlagBits::eVertex,
                0,
                sizeof(glm::mat4x4),
                &triangle_transform
            );
            cmd.bindVertexBuffers(0, vertex_buffer_.buffer, {{0}});
            cmd.bindIndexBuffer(index_buffer_.buffer, 0, vk::IndexType::eUint16);
            cmd.drawIndexed(3, 1, 0, 0, 0);
        }
        cmd.endRendering();
    }

    vee::vulkan::transition_image(
        cmd,
        game_image_.image,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::eTransferSrcOptimal
    );

    // swapchain image transition
    vee::vulkan::transition_image(
        cmd,
        ctx_->swapchain.images[swapchain_idx],
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal
    );

    {
        vk::ImageBlit2 region = {
            vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1},
            {vk::Offset3D(),
             vk::Offset3D{
                 static_cast<int32_t>(game_image_.width()),
                 static_cast<int32_t>(game_image_.height()),
                 1
             }},
            vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1},
            {vk::Offset3D(),
             vk::Offset3D{
                 static_cast<int32_t>(ctx_->swapchain.width),
                 static_cast<int32_t>(ctx_->swapchain.height),
                 1
             }},
        };
        vk::BlitImageInfo2 blit_info = {
            game_image_.image,
            vk::ImageLayout::eTransferSrcOptimal,
            ctx_->swapchain.images[swapchain_idx],
            vk::ImageLayout::eTransferDstOptimal,
            region,
        };
        cmd.blitImage2(blit_info);
    }

    // swapchain image transition
    vee::vulkan::transition_image(
        cmd,
        ctx_->swapchain.images[swapchain_idx],
        vk::ImageLayout::eTransferDstOptimal,
        vk::ImageLayout::eColorAttachmentOptimal
    );
}
} // namespace ht