//    Copyright 2025 Steven Casper
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.


#pragma once

#include "Buffer.hpp"
#include "RingBuffer.hpp"
#include "Swapchain.hpp"

#include <functional>
#include <VkBootstrap.h>
#include <vulkan/vulkan.hpp>

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
#if VEE_DEBUG
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

    vk::PipelineCache pipeline_cache;
    vk::DescriptorPool descriptor_pool;

    Buffer staging_buffer;
    Buffer vertex_buffer;
    Buffer index_buffer;
};
}; // namespace vee
