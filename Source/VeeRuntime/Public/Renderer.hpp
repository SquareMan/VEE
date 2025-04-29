//
// Created by Square on 9/28/2024.
//

#pragma once

#include "Renderer/IRenderer.hpp"
#include "Renderer/Pipeline.hpp"
#include "Renderer/RenderCtx.hpp"

#include <functional>

namespace vee {
class RenderCtx;
struct Vertex;
class Renderer final {
public:
    explicit Renderer(const platform::Window& window);
    ~Renderer();
    void init();

    RenderCtx& get_ctx();

    void Render();

    void push_renderer(std::shared_ptr<IRenderer>&& renderer) {
        renderers_.emplace_back(std::move(renderer));
    }

private:
    void record_commands(vk::CommandBuffer cmd, const std::function<void(vk::CommandBuffer cmd)>& func);

    RenderCtx render_ctx_;
    std::vector<std::shared_ptr<IRenderer>> renderers_;
};
} // namespace vee