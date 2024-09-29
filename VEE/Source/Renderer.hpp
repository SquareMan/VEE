//
// Created by Square on 9/28/2024.
//

#ifndef RENDERER_H
#define RENDERER_H

#ifdef WIN32
#include "Windows.h"
#endif

#define VK_NO_PROTOTYPES
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Vee {
#ifdef WIN32
    using Window = HWND;
#endif

    class Renderer final {
    public:
        explicit Renderer(const Window &Window);

        ~Renderer();

        void Render();

    private:
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
        VkPipeline pipeline = VK_NULL_HANDLE;

        VkSemaphore acquire_semaphore = VK_NULL_HANDLE;
        VkSemaphore submit_semaphore = VK_NULL_HANDLE;
    };
} // Vee

#endif //RENDERER_H
