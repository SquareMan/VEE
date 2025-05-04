//
// Created by Square on 10/27/2024.
//

#pragma once

#include "Renderer/Buffer.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/IRenderer.hpp"
#include "Renderer/Pipeline.hpp"

namespace vee {
class Material;

class GameRenderer : public vee::IRenderer {
public:
    void on_init(RenderCtx& ctx) override;
    void on_render(vk::CommandBuffer cmd, uint32_t swapchain_idx) override;
    void on_destroy() override {}

private:
    vee::Buffer vertex_buffer_;
    vee::Buffer index_buffer_;

    vee::Image game_image_;
};

} // namespace vee
