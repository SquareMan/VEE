//
// Created by Square on 10/11/2024.
//

module;
#include <compare>
module Vee.Renderer;

import vulkan_hpp;
import vk_mem_alloc_hpp;
import VkUtil;

namespace vee {
Image::Image(vk::Device device, vma::Allocator allocator, vk::ImageUsageFlags usage_flags, vk::Extent3D extent, vk::Format format, vk::ImageAspectFlags aspect_flags)
    : extent(extent)
    , format(format)
    , device_(device)
    , allocator_(allocator)
    , usage(usage_flags)
    , aspect(aspect_flags) {
    create_image();
}

Image::~Image() {
    free_resources();
}
void Image::resize(vk::Extent3D new_size) {
    free_resources();
    extent = new_size;
    create_image();
}

uint32_t Image::width() const {
    return extent.width;
}

uint32_t Image::height() const {
    return extent.height;
}

void Image::create_image() {
    const vk::ImageCreateInfo image_info = {{}, vk::ImageType::e2D, format, extent, 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, usage};

    constexpr vma::AllocationCreateInfo allocation_info = {{}, vma::MemoryUsage::eGpuOnly, {}, vk::MemoryPropertyFlagBits::eDeviceLocal};

    const auto image_alloc = allocator_.createImage(image_info, allocation_info).value;
    image = image_alloc.first;
    allocation = image_alloc.second;

    const vk::ImageViewCreateInfo view_info = {{}, image, vk::ImageViewType::e2D, format, {}, {aspect, 0, 1, 0, 1}};
    view = device_.createImageView(view_info).value;
}

void Image::free_resources() {
    device_.destroyImageView(view);
    allocator_.destroyImage(image, allocation);
    image = nullptr;
    allocation = nullptr;
    view = nullptr;
}

} // namespace vee