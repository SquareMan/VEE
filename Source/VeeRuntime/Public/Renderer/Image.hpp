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


#pragma once

#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

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
