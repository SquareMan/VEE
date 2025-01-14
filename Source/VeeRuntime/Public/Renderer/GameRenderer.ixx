//
// Created by Square on 10/27/2024.
//

module;
#include <memory>
export module GameRenderer;

import vulkan_hpp;
import Vee.Renderer;

namespace vee {

export class GameRenderer : public vee::IRenderer {
public:
    void on_init() override;
    void on_render(vk::CommandBuffer cmd, uint32_t swapchain_idx) override;
    void on_destroy() override {}

private:
    vee::vulkan::Pipeline sprite_pipeline_;

    vee::Buffer staging_buffer_;
    vee::Buffer vertex_buffer_;
    vee::Buffer index_buffer_;

    vee::Image game_image_;
    std::shared_ptr<vee::Image> tex_image_;

    vk::DescriptorPool descriptor_pool_;
    vk::DescriptorSet tex_descriptor_;
};

} // namespace vee
