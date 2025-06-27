//
// Created by Square on 6/14/2025.
//

#pragma once

namespace vee {
class RenderCtx;
}

namespace vee::rdg {
class Sink {
public:
    virtual ~Sink();

    /**
     * Called by the RenderGraph before Sources and Sinks are wired up to give a chance to
     * initialize and required graphics resources
     * @param ctx Engine global rendering context.
     */
    virtual void init(RenderCtx& ctx) {};
};
} // namespace vee::rdg
