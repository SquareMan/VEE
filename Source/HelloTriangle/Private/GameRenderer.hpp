//
// Created by Square on 10/27/2024.
//

#pragma once

#include "Renderer/Buffer.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/IRenderer.hpp"
#include "Renderer/Pipeline.hpp"

namespace ht {

class GameRenderer : public vee::IRenderer {
public:
    void on_init(std::shared_ptr<vee::RenderCtx>& ctx) override;
    void on_render(vk::CommandBuffer cmd, uint32_t swapchain_idx) override;
    void on_destroy() override {}

private:
    std::shared_ptr<vee::RenderCtx> ctx_;

    vee::vulkan::Pipeline triangle_pipeline_;
    vee::vulkan::Pipeline square_pipeline_;

    vee::Buffer staging_buffer_;
    vee::Buffer vertex_buffer_;
    vee::Buffer index_buffer_;

    vee::Image game_image_;
    vee::Image tex_image_;

    vk::DescriptorPool descriptor_pool_;
    vk::DescriptorSet tex_descriptor_;
};

} // namespace ht
