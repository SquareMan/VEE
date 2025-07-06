//
// Created by Square on 6/8/2025.
//

#include "Engine/FrameImagePass.hpp"

#include "IApplication.hpp"
#include "Renderer.hpp"
#include "Renderer/RenderCtx.hpp"
#include "RenderGraph/DirectSource.hpp"
#include "RenderGraph/ImageResource.hpp"

#include <entt/locator/locator.hpp>
#include <tracy/Tracy.hpp>


namespace vee::rdg {

void CopyBufferSink::init(RenderCtx& ctx) {
    for (int i = 0; i < resource_.size(); i++) {
        auto [buf, alloc] =
            ctx.allocator
                .createBuffer({{}, DebugScreen::WIDTH * DebugScreen::HEIGHT * 4, vk::BufferUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive}, {vma::AllocationCreateFlagBits::eHostAccessSequentialWrite, vma::MemoryUsage::eAuto})
                .value;

        void* mem = ctx.allocator.mapMemory(alloc).value;
        resource_[i] = std::make_shared<DebugBuffer>(std::move(Buffer(buf, alloc, ctx.allocator)), ctx.allocator, mem);
    }
}
void CopyBufferSink::prepare(const RenderGraph& ctx) {
    const Renderer& renderer = entt::locator<IApplication>::value().get_renderer();
    const uint64_t idx = renderer.get_frame_number() % 3;

    target = resource_[idx];
}

void CopyDestSink::init(RenderCtx& ctx) {
    for (int i = 0; i < resource_.size(); i++) {
        resource_[i] = std::make_unique<Image>(
            ctx.device,
            ctx.allocator,
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc,
            vk::Extent3D(DebugScreen::WIDTH, DebugScreen::HEIGHT, 1),
            vk::Format::eR8G8B8A8Srgb,
            vk::ImageAspectFlagBits::eColor
        );
    }
    target = std::make_shared<ImageResource>();
}

void CopyDestSink::prepare(const RenderGraph& ctx) {
    const Renderer& renderer = entt::locator<IApplication>::value().get_renderer();
    const uint64_t idx = renderer.get_frame_number() % 3;

    target->image = resource_[idx]->image;
    target->view = resource_[idx]->view;
    target->width = resource_[idx]->width();
    target->height = resource_[idx]->height();
}

vee::rdg::DebugBuffer::~DebugBuffer() {
    allocator.unmapMemory(buf.allocation);
}

FrameImageRenderPass::FrameImageRenderPass() {
    register_source("copy_source"_hash, DirectSource<ImageResource>::make(copy_source_));
    register_sink("copy_dest"_hash, CopyDestSink::make(copy_dest_));
    register_sink("copy_buffer"_hash, CopyBufferSink::make(copy_buffer_));
}

void FrameImageRenderPass::execute(vk::CommandBuffer cmd) {
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
} // namespace vee::rdg