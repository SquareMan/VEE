//
// Created by Square on 9/28/2024.
//

#pragma once

#include "Renderer/RenderCtx.hpp"

#include <functional>

namespace vee::rdg {
class RenderGraph;
}

namespace vee {
class RenderCtx;
struct Vertex;
class Renderer final {
public:
    explicit Renderer(const platform::Window& window);
    ~Renderer();

    /**
     * Set the RenderGraph that will be executed every frame. A RenderGraph is always expected to
     * exist before beginning a frame.
     * @param render_graph the RenderGraph
     */
    void set_render_graph(std::unique_ptr<rdg::RenderGraph>&& render_graph);

    RenderCtx& get_ctx();
    void render();

private:
    RenderCtx render_ctx_;
    std::unique_ptr<rdg::RenderGraph> render_graph_;
};
} // namespace vee