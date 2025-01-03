//
// Created by Square on 10/27/2024.
//

#pragma once
#include "Renderer/IRenderer.hpp"
#include <stdint.h>

import vulkan_hpp;

namespace vee {

class ImguiRenderer : public IRenderer {
public:
    void on_init() override;
    void on_render(vk::CommandBuffer cmd, uint32_t swapchain_idx) override;
    void on_destroy() override;
};

} // namespace vee
