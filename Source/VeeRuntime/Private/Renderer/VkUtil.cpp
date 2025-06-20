//
// Created by Square on 10/1/2024.
//

#define VMA_IMPLEMENTATION
#include "Renderer/VkUtil.hpp"

#include "Logging.hpp"
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
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        log_error(pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        log_warning(pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        log_info(pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        log_debug(pCallbackData->pMessage);
    }

    VEE_DEBUGBREAK();
    return VK_FALSE;
}

} // namespace vee::vulkan