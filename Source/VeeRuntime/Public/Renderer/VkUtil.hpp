//
// Created by Square on 10/1/2024.
//

#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <vulkan/vulkan.hpp>

// include vk_mem_alloc.h ourselves to avoid search path issues
// clang-format off
#include <vma/vk_mem_alloc.h>
#include <vk_mem_alloc.hpp>
// clang-format on

#include "VeeCore.hpp"
#include <vector>

namespace vee::vulkan {
[[nodiscard]] std::vector<const char*> filter_extensions(std::vector<const char*>& available_extensions, std::vector<const char*>& requested_extensions);

VkBool32 vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData
);

void transition_image(vk::CommandBuffer cmd, vk::Image image, vk::ImageLayout from, vk::ImageLayout to);
} // namespace vee::vulkan