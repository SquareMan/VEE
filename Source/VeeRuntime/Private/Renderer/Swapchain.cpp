//    Copyright 2025 Steven Casper
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.


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
#if defined(TRACY_ENABLE) && !defined(TRACY_NO_FRAME_IMAGE)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
#endif
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