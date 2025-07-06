//
// Created by Square on 10/10/2024.
//

#include "Renderer/Buffer.hpp"

namespace vee {
Buffer::Buffer() = default;
Buffer::Buffer(Buffer&& other) {
    *this = std::move(other);
}
Buffer& Buffer::operator=(Buffer&& other) {
    buffer = other.buffer;
    allocation = other.allocation;
    allocator = other.allocator;

    other.buffer = nullptr;
    other.allocation = nullptr;
    other.allocator = nullptr;

    return *this;
};

Buffer::~Buffer() {
    if (buffer != nullptr) {
        allocator.destroyBuffer(buffer, allocation);
    }
}
} // namespace vee