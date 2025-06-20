//
// Created by Square on 6/8/2025.
//

#include "Engine/FrameImagePass.hpp"

#include "Renderer/RenderCtx.hpp"
#include "RenderGraph/DirectSource.hpp"
#include "RenderGraph/ImageResource.hpp"

#include <tracy/Tracy.hpp>


vee::rdg::DebugBuffer::~DebugBuffer() {
    allocator.unmapMemory(buf.allocation);
}

vee::rdg::FrameImageRenderPass::FrameImageRenderPass() {
    register_source("copy_source", DirectSource<ImageResource>::make(copy_source_));
    register_sink("copy_dest", CopyDestSink::make(copy_dest_));
    register_sink("copy_buffer", CopyBufferSink::make(copy_buffer_));
}

void vee::rdg::FrameImageRenderPass::execute(vk::CommandBuffer cmd) {
    ZoneScoped;

    {
        const vk::ImageMemoryBarrier2 image_barrier[] = {
            {vk::PipelineStageFlagBits2::eColorAttachmentOutput,
             vk::AccessFlagBits2::eColorAttachmentWrite,
             vk::PipelineStageFlagBits2::eTransfer,
             vk::AccessFlagBits2::eTransferRead,
             vk::ImageLayout::eColorAttachmentOptimal,
             vk::ImageLayout::eTransferSrcOptimal,
             {},
             {},
             copy_source_->image,
             {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}},
            {vk::PipelineStageFlagBits2::eNone,
             vk::AccessFlagBits2::eNone,
             vk::PipelineStageFlagBits2::eTransfer,
             vk::AccessFlagBits2::eTransferWrite,
             vk::ImageLayout::eUndefined,
             vk::ImageLayout::eTransferDstOptimal,
             {},
             {},
             copy_dest_->image,
             {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}}
        };

        vk::DependencyInfo dependency_info;
        dependency_info.setImageMemoryBarriers(image_barrier);
        cmd.pipelineBarrier2(dependency_info);
    }

    // For Tracy, we need to save a copy of the framebuffer to the CPU. However, it needs to be
    // downscaled for better transfer performance, so we will need to blit to an intermediate
    // image to copy from
    const auto blit = vk::ImageBlit2(
        vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
        {vk::Offset3D{0, 0, 0},
         vk::Offset3D{static_cast<int32_t>(copy_source_->width), static_cast<int32_t>(copy_source_->height), 1}},
        {vk::ImageAspectFlagBits::eColor, 0, 0, 1},
        {vk::Offset3D{0, 0, 0}, vk::Offset3D{DebugScreen::WIDTH, DebugScreen::HEIGHT, 1}}
    );
    auto info = vk::BlitImageInfo2(
        copy_source_->image, vk::ImageLayout::eTransferSrcOptimal, copy_dest_->image, vk::ImageLayout::eTransferDstOptimal, blit, vk::Filter::eNearest
    );
    cmd.blitImage2(info);
    {
        const vk::ImageMemoryBarrier2 image_barrier[] = {
            {vk::PipelineStageFlagBits2::eTransfer,
             vk::AccessFlagBits2::eTransferRead,
             vk::PipelineStageFlagBits2::eAllCommands,
             vk::AccessFlagBits2::eColorAttachmentWrite | vk::AccessFlagBits2::eColorAttachmentRead,
             vk::ImageLayout::eTransferSrcOptimal,
             vk::ImageLayout::eColorAttachmentOptimal,
             {},
             {},
             copy_source_->image,
             {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}},
            {vk::PipelineStageFlagBits2::eTransfer,
             vk::AccessFlagBits2::eTransferWrite,
             vk::PipelineStageFlagBits2::eTransfer,
             vk::AccessFlagBits2::eTransferRead,
             vk::ImageLayout::eTransferDstOptimal,
             vk::ImageLayout::eTransferSrcOptimal,
             {},
             {},
             copy_dest_->image,
             {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}}
        };

        vk::DependencyInfo dependency_info;
        dependency_info.setImageMemoryBarriers(image_barrier);
        cmd.pipelineBarrier2(dependency_info);
    }

    cmd.copyImageToBuffer(
        copy_dest_->image,
        vk::ImageLayout::eTransferSrcOptimal,
        copy_buffer_->buf.buffer,
        vk::BufferImageCopy(0, DebugScreen::WIDTH, DebugScreen::HEIGHT, {vk::ImageAspectFlagBits::eColor, 0, 0, 1}, {0, 0, 0}, {DebugScreen::WIDTH, DebugScreen::HEIGHT, 1})
    );

    {
        const vk::BufferMemoryBarrier2 buffer_barrier = {
            vk::PipelineStageFlagBits2::eTransfer,
            vk::AccessFlagBits2::eTransferWrite,
            vk::PipelineStageFlagBits2::eHost,
            vk::AccessFlagBits2::eHostRead,
            {},
            {},
            copy_buffer_->buf.buffer,
            {},
            vk::WholeSize,
        };
        vk::DependencyInfo dependency_info;
        dependency_info.setBufferMemoryBarriers(buffer_barrier);
        cmd.pipelineBarrier2(dependency_info);
    }
}