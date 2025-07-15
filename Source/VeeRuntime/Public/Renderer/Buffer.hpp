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
