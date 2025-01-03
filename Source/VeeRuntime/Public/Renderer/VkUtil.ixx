//
// Created by Square on 10/1/2024.
//

module;
#include <vector>
#include <vulkan/vulkan.hpp>

export module VkUtil;
import vulkan_hpp;

export VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

export namespace vee::vulkan {
[[nodiscard]] std::vector<const char*> filter_extensions(std::vector<const char*>& available_extensions, std::vector<const char*>& requested_extensions);

vk::Bool32 vk_debug_callback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity, vk::DebugUtilsMessageTypeFlagsEXT messageTypes, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData
);

void transition_image(vk::CommandBuffer cmd, vk::Image image, vk::ImageLayout from, vk::ImageLayout to);
} // namespace vee::vulkan