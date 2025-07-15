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

#include <vulkan/vulkan.hpp>

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
