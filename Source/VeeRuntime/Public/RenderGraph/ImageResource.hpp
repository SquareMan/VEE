//
// Created by Square on 6/14/2025.
//

#pragma once

#include <cstdint>
#include <vulkan/vulkan.hpp>


namespace vee::rdg {
class ImageResource {
public:
    vk::Image image;
    vk::ImageView view;
    uint32_t width;
    uint32_t height;
};
} // namespace vee::rdg
