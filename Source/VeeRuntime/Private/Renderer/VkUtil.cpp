//
// Created by Square on 10/1/2024.
//

#define VMA_IMPLEMENTATION
#include "Renderer/VkUtil.hpp"

#include "VeeCore.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>

#include <vulkan/vulkan.hpp>

// Implement storage for Vulkan-hpp's dynamic loader
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace vee::vulkan {
std::vector<const char*> filter_extensions(std::vector<const char*>& available_extensions, std::vector<const char*>& requested_extensions) {
    std::ranges::sort(available_extensions, [](const char* a, const char* b) {
        return std::strcmp(a, b) < 0;
    });
    std::ranges::sort(requested_extensions, [](const char* a, const char* b) {
        return std::strcmp(a, b) < 0;
    });

    std::vector<const char*> result;
    std::ranges::set_intersection(requested_extensions, available_extensions, std::back_inserter(result), [](const char* a, const char* b) {
        return std::strcmp(a, b) == 0;
    });

    return result;
}
VkBool32 vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData
) {
    std::cout << pCallbackData->pMessage << std::endl;
    VEE_DEBUGBREAK();
    return VK_FALSE;
}

void transition_image(vk::CommandBuffer cmd, vk::Image image, vk::ImageLayout from, vk::ImageLayout to) {
    // FIXME: This is the most inefficient transition
    vk::ImageMemoryBarrier2 image_barrier = {
        vk::PipelineStageFlagBits2::eAllCommands,
        vk::AccessFlagBits2::eMemoryWrite,
        vk::PipelineStageFlagBits2::eAllCommands,
        vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead,
        from,
        to,
        {},
        {},
        image,
        {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
    };

    vk::DependencyInfo dependency_info = {};
    dependency_info.setImageMemoryBarriers(image_barrier);
    cmd.pipelineBarrier2(dependency_info);
}
} // namespace vee::vulkan