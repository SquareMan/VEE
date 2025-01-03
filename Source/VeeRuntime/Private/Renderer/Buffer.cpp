//
// Created by Square on 10/10/2024.
//

#include <compare>
#include "Renderer/Buffer.hpp"

namespace vee {
Buffer::~Buffer() {
    allocator.destroyBuffer(buffer, allocation);
}
} // namespace vee