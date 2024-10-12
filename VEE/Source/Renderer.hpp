//
// Created by Square on 9/28/2024.
//

#pragma once

#include "Platform/Window.hpp"
#include "Renderer/Buffer.hpp"
#include "Renderer/Instance.hpp"
#include "Renderer/Pipeline.hpp"
#include "Renderer/Swapchain.hpp"
#include "Renderer/VkUtil.hpp"
#include "RingBuffer.hpp"

#include <optional>
#include <vector>

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
    void recreate_swapchain();

    const Platform::Window* window;

    // Fixme: this is ugly, but we have to defer intialization of this to the constructor body
    std::optional<Vulkan::Instance> instance;
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
    vk::RenderPass render_pass;

    RingBuffer<CmdBuffer, 3> command_buffers;

    Vee::Buffer staging_buffer;
    Vee::Buffer vertex_buffer;
    Vee::Buffer index_buffer;
};
} // namespace Vee