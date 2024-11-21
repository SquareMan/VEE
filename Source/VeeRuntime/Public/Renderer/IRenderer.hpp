//
// Created by Square on 10/27/2024.
//

#pragma once

#include "VkUtil.hpp"

namespace vee {
class RenderCtx;

class IRenderer {
public:
    IRenderer() = default;
    IRenderer(const IRenderer&) = default;
    IRenderer(IRenderer&&) = default;
    IRenderer& operator=(const IRenderer&) = default;
    IRenderer& operator=(IRenderer&&) = default;
    virtual ~IRenderer() = default;

    virtual void on_init() = 0;
    virtual void on_render(vk::CommandBuffer cmd, uint32_t swapchain_idx) = 0;
    virtual void on_destroy() = 0;
};
} // namespace vee
