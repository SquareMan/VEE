//
// Created by Square on 6/6/2025.
//

#pragma once

#include "Assert.hpp"
#include "RenderGraph/Handles.hpp"
#include "RenderGraph/Pass.hpp"
#include "RenderGraph/Sink.hpp"
#include "RenderGraph/Source.hpp"


#include <vulkan/vulkan.hpp>

namespace vee {
class Buffer;
class RenderCtx;
} // namespace vee
namespace vee::rdg {
class ImageResource;

/**
 * PassHandle for referring to Sinks/Sources owned by the RenderGraph itself.
 */
static const PassHandle GLOBAL = "";

class RenderGraph;

// Add all the passes here
class RenderGraphBuilder {
public:
    /**
     * Creates a new empty Pass.
     * @param name a name used to refer to this pass from other passes. Pass names must be unique
     * within a RenderGraph.
     * @param args Arguments to forward to the Pass constructor
     * @return Reference to the newly created pass. Returned reference may be invalidated if this
     * function is called again.
     */
    template <typename T, typename... Args>
        requires std::is_base_of_v<Pass, T>
    Pass& add_pass(PassHandle name, Args&&... args) {
        VASSERT(!passes_.contains(name), "Attempted to add two RenderGraph passes with the same name");
        std::unique_ptr<Pass> pass = std::make_unique<T>(std::forward<Args>(args)...);
        pass_order_.push_back(name);
        return *passes_.insert({name, std::move(pass)}).first->second;
    }

    /**
     * Compile a RenderGraph. This builder is invalidated afterwards.
     * @param render_ctx Engine global rendering context
     * @return Compiled RenderGraph
     */
    RenderGraph build(RenderCtx& render_ctx);

protected:
    std::unordered_map<PassHandle, std::unique_ptr<Pass>> passes_;
    std::vector<PassHandle> pass_order_;
};

// A compiled/built RenderGraph
class RenderGraph {
public:
    RenderGraph(std::unordered_map<PassHandle, std::unique_ptr<Pass>>&& passes, std::vector<PassHandle>&& pass_order, RenderCtx& ctx);
    RenderGraph(RenderGraph&& other) = default;
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
