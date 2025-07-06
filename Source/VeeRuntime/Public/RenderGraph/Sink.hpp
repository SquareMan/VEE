//
// Created by Square on 6/14/2025.
//

#pragma once

namespace vee {
class RenderCtx;
}

namespace vee::rdg {
class RenderGraph;
class Sink {
public:
    virtual ~Sink();

    /**
     * Called by the RenderGraph before Sources and Sinks are wired up to give a chance to
     * initialize and required graphics resources
     * @param ctx Engine global rendering context.
     */
    virtual void init(RenderCtx& ctx) {};

    /**
     * Called by the RenderGraph each frame before execution.
     * @param ctx
     */
    virtual void prepare(const RenderGraph& ctx) {};
};
} // namespace vee::rdg
