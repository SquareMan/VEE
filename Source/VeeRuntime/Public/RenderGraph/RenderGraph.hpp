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

#include "RenderGraph/Handles.hpp"

#include <memory>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.hpp>


namespace vee {
class Buffer;
class RenderCtx;
} // namespace vee
namespace vee::rdg {
class ImageResource;
class Pass;
class Sink;

/**
 * PassHandle for referring to Sinks/Sources owned by the RenderGraph itself.
 */
extern const Name GLOBAL;

/**
 * A compiled/built RenderGraph
 */
class RenderGraph {
public:
    RenderGraph(std::unordered_map<PassHandle, std::unique_ptr<Pass>>&& passes, std::vector<PassHandle>&& pass_order, RenderCtx& ctx);
    ~RenderGraph();

    RenderGraph(RenderGraph&& other);
    RenderGraph& operator=(RenderGraph&& other);
    RenderGraph(const RenderGraph&) = delete;
    RenderGraph& operator=(const RenderGraph&) = delete;

    void execute(RenderCtx& render_ctx) const;

    /**
     * Find a sink owned by a Pass belonging to this RenderGraph.
     * @param ref Reference to the Sink
     * @return Sink, if found.
     */
    Sink* find_sink(SinkRef ref) const;

protected:
    std::unordered_map<PassHandle, std::unique_ptr<Pass>> passes_;
    std::vector<PassHandle> pass_order_;

    std::unordered_map<SinkHandle, std::unique_ptr<Sink>> global_sinks_;

    std::shared_ptr<ImageResource> framebuffer_;
    std::shared_ptr<Buffer> vertex_buffer_;
    std::shared_ptr<Buffer> index_buffer_;

    vk::Semaphore buffer_semaphore_;
};

} // namespace vee::rdg
