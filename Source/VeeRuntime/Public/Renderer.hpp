//
// Created by Square on 9/28/2024.
//

#pragma once

#include "Renderer/Image.hpp"
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

#ifdef TRACY_ENABLE && !TRACY_NO_FRAME_IMAGE
    struct DebugScreen {
        static constexpr uint32_t WIDTH = 92;
        static constexpr uint32_t HEIGHT = 92;

        vk::Image image;
        vma::Allocation alloc;
        Buffer buf;
        void* mem;
    };
    std::array<DebugScreen, 3> debug_screens_;
#endif
};
} // namespace vee