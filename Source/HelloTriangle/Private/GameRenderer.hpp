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
    void OnInit(std::shared_ptr<vee::RenderCtx>& ctx) override;
    void OnRender(vk::CommandBuffer cmd, uint32_t swapchain_idx) override;
    void OnDestroy() override{};

private:
    std::shared_ptr<vee::RenderCtx> ctx_;

    vee::vulkan::Pipeline triangle_pipeline;
    vee::vulkan::Pipeline square_pipeline;

    vee::Buffer staging_buffer;
    vee::Buffer vertex_buffer;
    vee::Buffer index_buffer;

    vee::Image game_image_;
    vee::Image tex_image_;

    vk::DescriptorPool descriptor_pool_;
    vk::DescriptorSet tex_descriptor_;
};

} // namespace ht
