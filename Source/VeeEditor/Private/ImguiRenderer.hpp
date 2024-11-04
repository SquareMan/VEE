//
// Created by Square on 10/27/2024.
//

#pragma once
#include "Renderer/IRenderer.hpp"

namespace vee {

class ImguiRenderer : public IRenderer {
public:
    void on_init(std::shared_ptr<RenderCtx>& ctx) override;
    void on_render(vk::CommandBuffer cmd, uint32_t swapchain_idx) override;
    void on_destroy() override;

private:
    std::shared_ptr<RenderCtx> ctx_;
};

} // namespace vee
