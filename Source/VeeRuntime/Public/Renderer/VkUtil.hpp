//
// Created by Square on 10/1/2024.
//

#pragma once

#include <vk_mem_alloc.hpp>

#include <vector>

namespace vee::vulkan {
[[nodiscard]] std::vector<const char*> filter_extensions(std::vector<const char*>& available_extensions, std::vector<const char*>& requested_extensions);

VkBool32 vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData
);

void transition_image(vk::CommandBuffer cmd, vk::Image image, vk::ImageLayout from, vk::ImageLayout to);
} // namespace vee::vulkan