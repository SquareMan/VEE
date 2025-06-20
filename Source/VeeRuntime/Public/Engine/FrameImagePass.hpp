//
// Created by Square on 6/8/2025.
//

#pragma once

#include "Renderer/Buffer.hpp"
#include "RenderGraph/Pass.hpp"
#include "RenderGraph/Sink.hpp"

#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>


// TODO: Remove these when removing the temporary sink/source
#include "MakeSharedEnabler.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/RenderCtx.hpp"
#include "RenderGraph/ImageResource.hpp"

namespace vee::rdg {
namespace DebugScreen {
static constexpr uint32_t WIDTH = 92;
static constexpr uint32_t HEIGHT = 92;
} // namespace DebugScreen

// FIXME: This is some temporary stuff to get the initial render graph working. Refactor this pass
// into a combination of Blit and Copy nodes
struct DebugBuffer {
    Buffer buf;
    vma::Allocator allocator;
    void* mem;
    ~DebugBuffer();
};
class CopyBufferSink : public Sink {
public:
    static std::unique_ptr<CopyBufferSink> make(std::shared_ptr<DebugBuffer>& target) {
        return std::make_unique<MakeSharedEnabler<CopyBufferSink>>(target);
    }
    std::shared_ptr<DebugBuffer>& target;

    void init(RenderCtx& ctx) override {
        auto [buf, alloc] =
            ctx.allocator
                .createBuffer({{}, DebugScreen::WIDTH * DebugScreen::HEIGHT * 4, vk::BufferUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive}, {vma::AllocationCreateFlagBits::eHostAccessSequentialWrite, vma::MemoryUsage::eAuto})
                .value;
        target = std::make_shared<DebugBuffer>();
        new (&target->buf) Buffer(buf, alloc, ctx.allocator);
        target->mem = ctx.allocator.mapMemory(alloc).value;
        target->allocator = ctx.allocator;
    }

protected:
    explicit CopyBufferSink(std::shared_ptr<DebugBuffer>& target)
        : Sink()
        , target(target) {}
};

class CopyDestSink : public Sink {
public:
    static std::unique_ptr<CopyDestSink> make(std::shared_ptr<ImageResource>& target) {
        return std::make_unique<MakeSharedEnabler<CopyDestSink>>(target);
    }
    std::shared_ptr<ImageResource>& target;

    void init(RenderCtx& ctx) override {
        resource = std::make_unique<Image>(
            ctx.device,
            ctx.allocator,
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc,
            vk::Extent3D(DebugScreen::WIDTH, DebugScreen::HEIGHT, 1),
            vk::Format::eR8G8B8A8Srgb,
            vk::ImageAspectFlagBits::eColor
        );
        target = std::make_shared<ImageResource>();
        target->image = resource->image;
        target->view = resource->view;
        target->width = resource->width();
        target->height = resource->height();
    }

protected:
    explicit CopyDestSink(std::shared_ptr<ImageResource>& target)
        : Sink()
        , target(target) {}

    std::unique_ptr<Image> resource;
};

class ImageResource;

// std::array<DebugScreen, 3> debug_screens_;
class GraphCtx;

class FrameImageRenderPass : public Pass {
public:
    FrameImageRenderPass();

    void execute(vk::CommandBuffer cmd) override;

protected:
    std::shared_ptr<ImageResource> copy_source_;
    std::shared_ptr<ImageResource> copy_dest_;
    std::shared_ptr<DebugBuffer> copy_buffer_;
};

} // namespace vee::rdg
