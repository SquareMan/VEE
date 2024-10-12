//
// Created by Square on 10/8/2024.
//

#include "Swapchain.hpp"

namespace Vee {
Swapchain::Swapchain(
    vk::PhysicalDevice gpu,
    vk::Device device,
    vk::SurfaceKHR surface,
    vk::Format format,
    vk::RenderPass render_pass,
    uint32_t width,
    uint32_t height
)
    : format(format)
    , width(width)
    , height(height)
    , device(device) {
    // swapchain
    vk::SurfaceCapabilitiesKHR surface_capabilities = gpu.getSurfaceCapabilitiesKHR(surface).value;

    uint32_t img_count = surface_capabilities.minImageCount + 1;
    img_count = img_count > surface_capabilities.maxImageCount ? img_count - 1 : img_count;

    const vk::SwapchainCreateInfoKHR swapchain_info(
        {},
        surface,
        img_count,
        format,
        {},
        surface_capabilities.currentExtent,
        1,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst,
        {},
        {},
        surface_capabilities.currentTransform,
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        // TODO: Allow eMailbox in the future, maybe make it the default
        vk::PresentModeKHR::eFifoRelaxed
    );
    handle = device.createSwapchainKHR(swapchain_info).value;


    images = device.getSwapchainImagesKHR(handle).value;

    image_views.reserve(images.size());
    vk::ImageViewCreateInfo image_view_info(
        {}, {}, vk::ImageViewType::e2D, format, {}, {vk::ImageAspectFlagBits::eColor, {}, 1, {}, 1}
    );
    for (vk::Image swapchain_image : images) {
        image_view_info.image = swapchain_image,
        image_views.push_back(device.createImageView(image_view_info).value);
    }


    // framebuffer
    vk::FramebufferCreateInfo framebuffer_info({}, render_pass, {}, width, height, 1);

    framebuffers.reserve(images.size());
    for (uint32_t i = 0; i < images.size(); i++) {
        framebuffer_info.setAttachments(image_views[i]);
        framebuffers.push_back(device.createFramebuffer(framebuffer_info).value);
    }
}

Swapchain::~Swapchain() {
    for (const vk::Framebuffer framebuffer : framebuffers) {
        device.destroyFramebuffer(framebuffer);
    }
    for (const vk::ImageView view : image_views) {
        device.destroyImageView(view);
    }
    device.destroySwapchainKHR(handle);
}
} // namespace Vee