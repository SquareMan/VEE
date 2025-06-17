//
// Created by Square on 6/8/2025.
//

#include "Engine/FrameImagePass.hpp"

#include "Renderer/Image.hpp"
#include "Renderer/RenderCtx.hpp"
#include "RenderGraph/DirectSource.hpp"
#include "RenderGraph/ImageResource.hpp"
#include "RenderGraph/Sink.hpp"


vee::rdg::DebugBuffer::~DebugBuffer() {
    allocator.unmapMemory(buf.allocation);
}

vee::rdg::FrameImageRenderPass::FrameImageRenderPass() {
    register_source("copy_source", DirectSource<ImageResource>::make(copy_source_));
    register_sink("copy_dest", CopyDestSink::make(copy_dest_));
    register_sink("copy_buffer", CopyBufferSink::make(copy_buffer_));
}

void vee::rdg::FrameImageRenderPass::execute(vk::CommandBuffer cmd) {
    vulkan::transition_image(cmd, copy_source_->image, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eTransferSrcOptimal);
    vulkan::transition_image(cmd, copy_dest_->image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

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
    vulkan::transition_image(cmd, copy_dest_->image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal);
    cmd.copyImageToBuffer(
        copy_dest_->image,
        vk::ImageLayout::eTransferSrcOptimal,
        copy_buffer_->buf.buffer,
        vk::BufferImageCopy(0, DebugScreen::WIDTH, DebugScreen::HEIGHT, {vk::ImageAspectFlagBits::eColor, 0, 0, 1}, {0, 0, 0}, {DebugScreen::WIDTH, DebugScreen::HEIGHT, 1})
    );

    vulkan::transition_image(cmd, copy_source_->image, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eColorAttachmentOptimal);
}