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


#include "Engine/Texture.hpp"

#include "IApplication.hpp"
#include "Logging.hpp"
#include "MakeSharedEnabler.hpp"
#include "Renderer.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/RenderCtx.hpp"

#include <entt/locator/locator.hpp>
#include <glm/packing.hpp>
#include <stb_image.h>

std::expected<std::shared_ptr<vee::Texture>, vee::Texture::CreateError> vee::Texture::create(const char* path, vk::Format format) {
    uint32_t width, height, channels;
    if (!stbi_info(path, reinterpret_cast<int32_t*>(&width), reinterpret_cast<int32_t*>(&height), reinterpret_cast<int32_t*>(&channels))) {
        log_error("Unsupported texture format for \"{}\"\n{}", path, stbi_failure_reason());
        return std::unexpected(CreateError());
    }
    if (channels != 4) {
        log_error("Unsupported texture \"{}\" with < 4 channels", path);
        return std::unexpected(CreateError());
    }

    RenderCtx& ctx = entt::locator<IApplication>::value().get_renderer().get_ctx();
    std::shared_ptr<Texture> new_texture = nullptr;
    {
        auto data = std::unique_ptr<uint8_t, void (*)(void*)>(
            stbi_load(path, reinterpret_cast<int32_t*>(&width), reinterpret_cast<int32_t*>(&height), nullptr, STBI_rgb_alpha), stbi_image_free
        );
        if (data == nullptr) {
            log_error("Failed to load texture from file \"{}\"\n{}", path, stbi_failure_reason());
            return std::unexpected(CreateError());
        }

        new_texture = std::make_shared<MakeSharedEnabler<Texture>>();
        new_texture->image_ = std::make_shared<Image>(
            ctx.device, ctx.allocator, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, vk::Extent3D(width, height, 1), format, vk::ImageAspectFlagBits::eColor
        );

        // Copy loaded image data to VRAM and swap RGBA to BGRA to match expected GPU image format.
        // Note: we can probably optimize this by loading the image data directly into the staging
        // buffer.
        auto img_mem = static_cast<uint32_t*>(ctx.allocator.mapMemory(ctx.staging_buffer.allocation).value
        );
        for (std::uint32_t pixel = 0; pixel < width * height; pixel++) {
            const uint8_t* b = &data.get()[pixel * channels + 2];
            const uint8_t* g = &data.get()[pixel * channels + 1];
            const uint8_t* r = &data.get()[pixel * channels + 0];
            const uint8_t* a = &data.get()[pixel * channels + 3];

            img_mem[pixel] = glm::packUnorm4x8(glm::vec4(*b, *g, *r, *a) / 255.f);
        }

        ctx.allocator.unmapMemory(ctx.staging_buffer.allocation);
        std::ignore = ctx.allocator.flushAllocation(ctx.staging_buffer.allocation, 0, width * height * channels);
    }


    ctx.immediate_submit([&](vk::CommandBuffer cmd) {
        {
            const vk::ImageMemoryBarrier2 image_barrier = {
                vk::PipelineStageFlagBits2::eNone,
                vk::AccessFlagBits2::eNone,
                vk::PipelineStageFlagBits2::eTransfer,
                vk::AccessFlagBits2::eTransferWrite,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eTransferDstOptimal,
                {},
                {},
                new_texture->image_->image,
                {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
            };

            vk::DependencyInfo dependency_info;
            dependency_info.setImageMemoryBarriers(image_barrier);
            cmd.pipelineBarrier2(dependency_info);
        }

        vk::BufferImageCopy region = {{}, {}, {}, {vk::ImageAspectFlagBits::eColor, 0, 0, 1}, {}, {width, height, 1}};
        cmd.copyBufferToImage(ctx.staging_buffer.buffer, new_texture->image_->image, vk::ImageLayout::eTransferDstOptimal, region);

        {
            const vk::ImageMemoryBarrier2 image_barrier = {
                vk::PipelineStageFlagBits2::eTransfer,
                vk::AccessFlagBits2::eTransferWrite,
                vk::PipelineStageFlagBits2::eNone,
                vk::AccessFlagBits2::eNone,
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                {},
                {},
                new_texture->image_->image,
                {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
            };

            vk::DependencyInfo dependency_info;
            dependency_info.setImageMemoryBarriers(image_barrier);
            cmd.pipelineBarrier2(dependency_info);
        }
    });

    return new_texture;
}