//
// Created by Square on 10/10/2024.
//

#pragma once

#include "VkUtil.hpp"

namespace vee {

class Buffer {
public:
    Buffer() = default;
    Buffer(vk::Buffer buffer, vma::Allocation allocation, vma::Allocator allocator)
        : buffer(buffer)
        , allocation(allocation)
        , allocator(allocator){};
    ~Buffer();

    vk::Buffer buffer;
    vma::Allocation allocation;

private:
    vma::Allocator allocator;
};

} // namespace Vee
