//
// Created by Square on 10/27/2024.
//

#pragma once

#include "RingBuffer.hpp"
#include "Swapchain.hpp"
#include "VkUtil.hpp"

#include <functional>
#include <VkBootstrap.h>

struct CmdBuffer {
    vk::CommandBuffer cmd;
    vk::Fence fence;
    vk::Semaphore acquire_semaphore;
    vk::Semaphore submit_semaphore;
};

namespace vee {
namespace platform {
class Window;
}

class RenderCtx {
public:
    explicit RenderCtx(const platform::Window& window);

    void recreate_swapchain();
    void immediate_submit(const std::function<void(vk::CommandBuffer cmd)>& func) const;

    const platform::Window* window;
    vkb::Instance instance;
#if _DEBUG
    vk::DebugUtilsMessengerEXT debug_messenger_;
#endif
    vk::PhysicalDevice gpu;
    vk::Device device;
    vk::SurfaceKHR surface;
    Swapchain swapchain;
    vk::Queue graphics_queue;
    vk::Queue presentation_queue;
    vk::CommandPool command_pool;
    vma::Allocator allocator;

    vk::CommandBuffer immediate_buffer_;
    vk::Fence immediate_fence_;

    RingBuffer<CmdBuffer, 3> command_buffers;
};
}; // namespace vee
