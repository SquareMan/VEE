//
// Created by Square on 10/8/2024.
//

#pragma once

#include "Renderer/VkUtil.hpp"

namespace vee {

class Swapchain {
public:
    Swapchain() = default;
    Swapchain(vk::PhysicalDevice gpu, vk::Device device, vk::SurfaceKHR surface, vk::Format format, uint32_t width, uint32_t height);
    ~Swapchain();

    vk::SwapchainKHR handle;
    std::vector<vk::Image> images;
    std::vector<vk::ImageView> image_views;

    vk::Format format;
    uint32_t width = 0;
    uint32_t height = 0;

private:
    vk::Device device;
};

} // namespace vee
