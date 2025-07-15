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

    void init(RenderCtx& ctx) override;
    void prepare(const RenderGraph& ctx) override;

protected:
    explicit CopyBufferSink(std::shared_ptr<DebugBuffer>& target)
        : Sink()
        , target(target) {}

    std::array<std::shared_ptr<DebugBuffer>, 3> resource_;
};

class CopyDestSink : public Sink {
public:
    static std::unique_ptr<CopyDestSink> make(std::shared_ptr<ImageResource>& target) {
        return std::make_unique<MakeSharedEnabler<CopyDestSink>>(target);
    }
    std::shared_ptr<ImageResource>& target;

    void init(RenderCtx& ctx) override;
    void prepare(const RenderGraph& ctx) override;


protected:
    explicit CopyDestSink(std::shared_ptr<ImageResource>& target)
        : Sink()
        , target(target) {}

    std::array<std::unique_ptr<Image>, 3> resource_;
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
