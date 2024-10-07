//
// Created by Square on 10/1/2024.
//

#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

#include "VeeCore.hpp"
#include <vector>

namespace Vee::Vulkan {
VEE_NODISCARD std::vector<const char*> filter_extensions(
    std::vector<const char*>& available_extensions, std::vector<const char*>& requested_extensions
);
} // namespace Vee::Vulkan