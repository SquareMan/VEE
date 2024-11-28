//
// Created by Square on 10/8/2024.
//

#include "Renderer/Swapchain.hpp"

#include <VkBootstrap.h>

namespace vee {
Swapchain::Swapchain(vk::PhysicalDevice gpu, vk::Device device, vk::SurfaceKHR surface, vk::Format format, uint32_t width, uint32_t height)
    : format(format)
    , width(width)
    , height(height)
    , device(device) {
    vkb::Swapchain swapchain =
        vkb::SwapchainBuilder(gpu, device, surface)
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_RELAXED_KHR)
            .set_desired_min_image_count(vkb::SwapchainBuilder::TRIPLE_BUFFERING)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .build()
            .value();

    handle = swapchain.swapchain;
    std::ranges::transform(swapchain.get_images().value(), std::back_inserter(images), [](const VkImage& image) {
        return image;
    });
    std::ranges::transform(swapchain.get_image_views().value(), std::back_inserter(image_views), [](const VkImageView& view) {
        return view;
    });
    format = static_cast<vk::Format>(swapchain.image_format);
    width = swapchain.extent.width;
    height = swapchain.extent.height;
}

Swapchain::~Swapchain() {
    for (const vk::ImageView view : image_views) {
        device.destroyImageView(view);
    }
    device.destroySwapchainKHR(handle);
}
} // namespace vee