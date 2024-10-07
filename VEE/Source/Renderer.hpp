//
// Created by Square on 9/28/2024.
//

#pragma once

#include "Platform/Window.hpp"
#include "Renderer/Instance.hpp"
#include "Renderer/Pipeline.hpp"
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
    const Platform::Window* window;

    // Fixme: this is ugly, but we have to defer intialization of this to the constructor body
    std::optional<Vulkan::Instance> instance;
    Vulkan::Pipeline triangle_pipeline;
    Vulkan::Pipeline square_pipeline;

    vk::PhysicalDevice gpu;
    vk::Device device;
    vk::Queue graphics_queue;
    vk::Queue presentation_queue;
    vk::CommandPool command_pool;
    vk::SwapchainKHR swapchain;
    std::vector<vk::Image> swapchain_images;
    std::vector<vk::ImageView> swapchain_image_views;
    vk::RenderPass render_pass;
    std::vector<vk::Framebuffer> framebuffers;

    RingBuffer<CmdBuffer, 3> command_buffers;

    vk::Buffer staging_buffer;
    vk::DeviceMemory staging_buffer_memory;
    vk::Buffer vertex_buffer;
    vk::DeviceMemory vertex_buffer_memory;
};
} // namespace Vee