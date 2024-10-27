//
// Created by Square on 10/27/2024.
//

#pragma once

#include "VkUtil.hpp"

namespace vee {
class RenderCtx;
}
namespace vee {
class IRenderer {
public:
    virtual void OnInit(std::shared_ptr<RenderCtx>& ctx) = 0;
    virtual void OnRender(vk::CommandBuffer cmd, uint32_t swapchain_idx) = 0;
    virtual void OnDestroy() = 0;
};
} // namespace vee
