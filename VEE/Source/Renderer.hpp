//
// Created by Square on 9/28/2024.
//

#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define VK_NO_PROTOTYPES
#include "RingBuffer.hpp"


#include <vector>
#include <vulkan/vulkan_core.h>

#include "Platform/Window.hpp"
#include "Renderer/Instance.hpp"
#include "Renderer/Pipeline.hpp"

#include <optional>

struct CmdBuffer {
    VkCommandBuffer cmd = VK_NULL_HANDLE;
    VkFence fence = VK_NULL_HANDLE;
    VkSemaphore acquire_semaphore = VK_NULL_HANDLE;
    VkSemaphore submit_semaphore = VK_NULL_HANDLE;
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

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphics_queue = VK_NULL_HANDLE;
    VkQueue presentation_queue = VK_NULL_HANDLE;
    VkCommandPool command_pool = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;
    VkRenderPass render_pass = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> framebuffers;

    RingBuffer<CmdBuffer, 3> command_buffers;

    VkBuffer staging_buffer = VK_NULL_HANDLE;
    VkDeviceMemory staging_buffer_memory = VK_NULL_HANDLE;
    VkBuffer vertex_buffer = VK_NULL_HANDLE;
    VkDeviceMemory vertex_buffer_memory = VK_NULL_HANDLE;
};
} // namespace Vee