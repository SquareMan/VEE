//
// Created by Square on 9/28/2024.
//

#pragma once

#include "Platform/Window.hpp"
#include "Renderer/Buffer.hpp"
#include "Renderer/Pipeline.hpp"
#include "Renderer/Swapchain.hpp"
#include "Renderer/VkUtil.hpp"
#include "RingBuffer.hpp"

#include <functional>
#include <optional>
#include <vector>

#include <VkBootstrap.h>

struct CmdBuffer {
    vk::CommandBuffer cmd;
    vk::Fence fence;
    vk::Semaphore acquire_semaphore;
    vk::Semaphore submit_semaphore;
};

namespace Vee {
struct Vertex;
class Renderer final {
public:
    explicit Renderer(const Platform::Window& Window);
    ~Renderer();

    void Render();

private:
    void init_imgui();

    void
    record_commands(vk::CommandBuffer cmd, const std::function<void(vk::CommandBuffer cmd)>& func);
    void render(
        vk::RenderingInfo& rendering_info,
        vk::CommandBuffer cmd,
        const std::function<void(vk::CommandBuffer cmd)>& func
    );
    void recreate_swapchain();

    const Platform::Window* window;

    vkb::Instance instance;
#if _DEBUG
    vk::DebugUtilsMessengerEXT debug_messenger_;
#endif

    Vulkan::Pipeline triangle_pipeline;
    Vulkan::Pipeline square_pipeline;

    vk::PhysicalDevice gpu;
    vk::Device device;
    vk::SurfaceKHR surface;
    std::optional<Swapchain> swapchain;
    vk::Queue graphics_queue;
    vk::Queue presentation_queue;
    vk::CommandPool command_pool;

    RingBuffer<CmdBuffer, 3> command_buffers;

    Vee::Buffer staging_buffer;
    Vee::Buffer vertex_buffer;
    Vee::Buffer index_buffer;
};
} // namespace Vee