//
// Created by Square on 10/11/2024.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.hpp>

namespace vee {

class Image {
public:
    Image() = default;
    Image(vk::Device device, vma::Allocator allocator, vk::ImageUsageFlags usage_flags, vk::Extent3D extent, vk::Format format, vk::ImageAspectFlags aspect_flags);
    ~Image();

    // Destructively resizes this image.
    // This will free the image resource on the GPU and create a new one in its place.
    // WARNING: The user must ensure that it's safe to deallocate this image before calling!
    void resize(vk::Extent3D new_size);

    uint32_t width() const;
    uint32_t height() const;

    vk::Image image;
    vma::Allocation allocation;
    vk::ImageView view;
    vk::Extent3D extent;
    vk::Format format;
    vk::ImageUsageFlags usage;
    vk::ImageAspectFlags aspect;

private:
    vk::Device device_;
    vma::Allocator allocator_;

    void create_image();
    void free_resources();
};

} // namespace vee
