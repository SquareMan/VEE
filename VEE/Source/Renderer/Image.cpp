//
// Created by Square on 10/11/2024.
//

#include "Image.hpp"

namespace Vee {
Image::Image(
    vk::Device device,
    vma::Allocator allocator,
    vk::ImageUsageFlags usage_flags,
    vk::Extent3D extent,
    vk::Format format,
    vk::ImageAspectFlags aspect_flags
)
    : extent(extent)
    , format(format)
    , device_(device)
    , allocator_(allocator) {
    const vk::ImageCreateInfo image_info = {
        {},
        vk::ImageType::e2D,
        format,
        extent,
        1,
        1,
        vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal,
        usage_flags
    };

    constexpr vma::AllocationCreateInfo allocation_info = {
        {}, vma::MemoryUsage::eGpuOnly, {}, vk::MemoryPropertyFlagBits::eDeviceLocal
    };

    const auto image_alloc = allocator.createImage(image_info, allocation_info).value;
    image = image_alloc.first;
    allocation = image_alloc.second;

    const vk::ImageViewCreateInfo view_info = {
        {}, image, vk::ImageViewType::e2D, format, {}, {aspect_flags, 0, 1, 0, 1}
    };
    image_view = device.createImageView(view_info).value;
}

Image::~Image() {
    device_.destroyImageView(image_view);
    allocator_.destroyImage(image, allocation);
}
} // namespace Vee