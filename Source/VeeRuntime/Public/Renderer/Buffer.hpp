//
// Created by Square on 10/10/2024.
//

#pragma once

#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

namespace vee {

class Buffer {
public:
    Buffer();
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&);
    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&);
    Buffer(vk::Buffer buffer, vma::Allocation allocation, vma::Allocator allocator)
        : buffer(buffer)
        , allocation(allocation)
        , allocator(allocator) {};
    ~Buffer();

    vk::Buffer buffer;
    vma::Allocation allocation;

private:
    vma::Allocator allocator;
};

} // namespace vee
