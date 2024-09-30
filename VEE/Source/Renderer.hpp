//
// Created by Square on 9/28/2024.
//

#pragma once

#define VK_NO_PROTOTYPES
#include <vector>
#include <vulkan/vulkan_core.h>

#include "Platform/Window.hpp"

namespace Vee {
class Renderer final {
public:
    explicit Renderer(const Platform::Window& Window);

    ~Renderer();

    void Render();

private:
    const Platform::Window* window;

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue queue = VK_NULL_HANDLE;
    VkCommandPool command_pool = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;
    VkRenderPass render_pass = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> framebuffers;
    VkPipelineCache pipeline_cache = VK_NULL_HANDLE;
    VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
    VkPipeline triangle_pipeline = VK_NULL_HANDLE;
    VkPipeline square_pipeline = VK_NULL_HANDLE;

    VkSemaphore acquire_semaphore = VK_NULL_HANDLE;
    VkSemaphore submit_semaphore = VK_NULL_HANDLE;

    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    VkFence submit_fence = VK_NULL_HANDLE;
};
} // namespace Vee