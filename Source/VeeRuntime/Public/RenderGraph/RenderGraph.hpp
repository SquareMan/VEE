//
// Created by Square on 6/6/2025.
//

#pragma once

#include "RenderGraph/Handles.hpp"

#include <memory>
#include <unordered_map>
#include <vector>


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
static const PassHandle GLOBAL = "";

/**
 * A compiled/built RenderGraph
 */
class RenderGraph {
public:
    RenderGraph(std::unordered_map<PassHandle, std::unique_ptr<Pass>>&& passes, std::vector<PassHandle>&& pass_order, RenderCtx& ctx);
    RenderGraph(RenderGraph&& other);
    ~RenderGraph();
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
};

} // namespace vee::rdg
