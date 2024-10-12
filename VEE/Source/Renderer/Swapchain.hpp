//
// Created by Square on 10/8/2024.
//

#pragma once

#include "Renderer/VkUtil.hpp"

namespace Vee {

class Swapchain {
public:
    Swapchain(
        vk::PhysicalDevice gpu,
        vk::Device device,
        vk::SurfaceKHR surface,
        vk::Format format,
        uint32_t width,
        uint32_t height
    );
    ~Swapchain();

    vk::SwapchainKHR handle;
    std::vector<vk::Image> images;
    std::vector<vk::ImageView> image_views;

    vk::Format format;
    uint32_t width;
    uint32_t height;

private:
    vk::Device device;
};

} // namespace Vee
