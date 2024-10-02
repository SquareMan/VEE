//
// Created by Square on 10/1/2024.
//

#pragma once

#include "VeeCore.hpp"
#include <vector>

#define VK_CHECK(func)                                                                             \
    {                                                                                              \
        const VkResult result = func;                                                              \
        if (result != VK_SUCCESS) {                                                                \
            std::cerr << "Error calling function " << #func << " at " << __FILE__ << ":"           \
                      << __LINE__ << ". Result is " << string_VkResult(result) << "\n";            \
            VEE_DEBUGBREAK();                                                                      \
        }                                                                                          \
    }

namespace Vee::Vulkan {
VEE_NODISCARD std::vector<const char*> filter_extensions(
    std::vector<const char*>& available_extensions, std::vector<const char*>& requested_extensions
);
} // namespace Vee::Vulkan