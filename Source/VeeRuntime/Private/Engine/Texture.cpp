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
    const uint8_t* data = stbi_load(path, reinterpret_cast<int32_t*>(&width), reinterpret_cast<int32_t*>(&height), reinterpret_cast<int32_t*>(&channels), STBI_rgb_alpha);

    if (data == nullptr) {
        log_error("Failed to load texture from file \"{}\"\n{}", path, stbi_failure_reason());
        return std::unexpected(CreateError());
    }

    Renderer& renderer = entt::locator<IApplication>::value().get_renderer();
    RenderCtx& ctx = renderer.get_ctx();

    std::shared_ptr<Texture> new_texture = std::make_shared<MakeSharedEnabler<Texture>>();
    new_texture->image_ = std::make_shared<Image>(
        ctx.device, ctx.allocator, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, vk::Extent3D(width, height, 1), format, vk::ImageAspectFlagBits::eColor
    );

    std::vector<uint32_t> pixels(width * height);
    for (std::uint32_t y = 0; y < height; y++) {
        for (std::uint32_t x = 0; x < width; x++) {
            const uint8_t* b = &data[(y * width + x) * 4 + 2];
            const uint8_t* g = &data[(y * width + x) * 4 + 1];
            const uint8_t* r = &data[(y * width + x) * 4 + 0];
            const uint8_t* a = &data[(y * width + x) * 4 + 3];

            pixels[y * width + x] = glm::packUnorm4x8(glm::vec4(*b, *g, *r, *a) / 255.f);
        }
    }

    std::ignore = ctx.allocator.copyMemoryToAllocation(pixels.data(), ctx.staging_buffer.allocation, 0, pixels.size() * sizeof(uint32_t));

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

        vk::BufferImageCopy region = {
            {}, {}, {}, {vk::ImageAspectFlagBits::eColor, 0, 0, 1}, {}, {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1}
        };
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

    // FIXME: we need to free the image data AFTER the transfer completes, but this will require
    // passing some kind of fence to the immediate submit function and then asynchronously waiting
    // on it.
    // stbi_image_free((void*)data);
    return new_texture;
}