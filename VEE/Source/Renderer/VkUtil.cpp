//
// Created by Square on 10/1/2024.
//

#include "VkUtil.hpp"

#include <algorithm>
#include <iterator>

#include <vulkan/vulkan.hpp>

// Implement storage for Vulkan-hpp's dynamic loader
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace Vee::Vulkan {
std::vector<const char*> filter_extensions(
    std::vector<const char*>& available_extensions, std::vector<const char*>& requested_extensions
) {
    std::ranges::sort(available_extensions, [](const char* a, const char* b) {
        return std::strcmp(a, b) < 0;
    });
    std::ranges::sort(requested_extensions, [](const char* a, const char* b) {
        return std::strcmp(a, b) < 0;
    });

    std::vector<const char*> result;
    std::ranges::set_intersection(
        requested_extensions,
        available_extensions,
        std::back_inserter(result),
        [](const char* a, const char* b) { return std::strcmp(a, b) == 0; }
    );

    return result;
}
} // namespace Vee::Vulkan