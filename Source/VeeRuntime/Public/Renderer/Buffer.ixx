//
// Created by Square on 10/10/2024.
//

module;
#include <compare>
export module Vee.Renderer:Buffer;
import vulkan_hpp;
import vk_mem_alloc_hpp;

namespace vee {

export class Buffer {
public:
    Buffer() = default;
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
