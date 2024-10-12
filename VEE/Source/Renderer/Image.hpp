//
// Created by Square on 10/11/2024.
//

#pragma once

#include "Renderer/VkUtil.hpp"

namespace Vee {

class Image {
public:
    Image() = default;
    Image(
        vk::Device device,
        vma::Allocator allocator,
        vk::ImageUsageFlags usage_flags,
        vk::Extent3D extent,
        vk::Format format,
        vk::ImageAspectFlags aspect_flags
    );
    ~Image();

    vk::Image image;
    vma::Allocation allocation;
    vk::ImageView image_view;
    vk::Extent3D extent;
    vk::Format format;

private:
    vk::Device device_;
    vma::Allocator allocator_;
};

} // namespace Vee
