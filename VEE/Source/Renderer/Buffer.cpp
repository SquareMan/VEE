//
// Created by Square on 10/10/2024.
//

#include "Buffer.hpp"

namespace Vee {
Buffer::~Buffer() {
    allocator.destroyBuffer(buffer, allocation);
}
} // Vee