//
// Created by Square on 10/27/2024.
//

module;
#include <stdint.h>
export module Vee.Renderer:IRenderer;

import vulkan_hpp;

namespace vee {
export class RenderCtx;

export class IRenderer {
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
