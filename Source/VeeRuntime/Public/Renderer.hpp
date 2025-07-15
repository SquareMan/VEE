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

#include "Renderer/RenderCtx.hpp"

#include <memory>

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

    std::uint64_t get_frame_number() const;

    /**
     * Set the RenderGraph that will be executed every frame. A RenderGraph is always expected to
     * exist before beginning a frame.
     * @param render_graph the RenderGraph
     */
    void set_render_graph(std::unique_ptr<rdg::RenderGraph>&& render_graph);

    RenderCtx& get_ctx();
    void render();

private:
    std::size_t frame_num_ = 0;

    RenderCtx render_ctx_;
    std::unique_ptr<rdg::RenderGraph> render_graph_;
};
} // namespace vee