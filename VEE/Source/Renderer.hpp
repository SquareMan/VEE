//
// Created by Square on 9/28/2024.
//

#pragma once

#include "Platform/Window.hpp"
#include "Renderer/Buffer.hpp"
#include "Renderer/Image.hpp"
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

namespace vee {
struct Vertex;
class Renderer final {
public:
    explicit Renderer(const platform::Window& Window);
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

    void transition_image(vk::CommandBuffer cmd, vk::Image image, vk::ImageLayout from, vk::ImageLayout to);

    const platform::Window* window;

    vkb::Instance instance;
#if _DEBUG
    vk::DebugUtilsMessengerEXT debug_messenger_;
#endif

    vulkan::Pipeline triangle_pipeline;
    vulkan::Pipeline square_pipeline;

    vk::PhysicalDevice gpu;
    vk::Device device;
    vk::SurfaceKHR surface;
    std::optional<Swapchain> swapchain;
    vk::Queue graphics_queue;
    vk::Queue presentation_queue;
    vk::CommandPool command_pool;

    vk::CommandBuffer immediate_buffer_;
    vk::Fence immediate_fence_;

    RingBuffer<CmdBuffer, 3> command_buffers;

    vee::Buffer staging_buffer;
    vee::Buffer vertex_buffer;
    vee::Buffer index_buffer;

    vee::Image game_image_;
};
} // namespace Vee