//
// Created by Square on 6/20/2025.
//

#include "RenderGraph/RenderGraphBuilder.hpp"

namespace vee::rdg {
RenderGraphBuilder::~RenderGraphBuilder() = default;

RenderGraph RenderGraphBuilder::build(RenderCtx& render_ctx) {
    return {std::move(passes_), std::move(pass_order_), render_ctx};
}
} // namespace vee::rdg
