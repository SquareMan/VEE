//
// Created by Square on 10/10/2024.
//

module;
#include <compare>
module Vee.Renderer;

namespace vee {
Buffer::~Buffer() {
    allocator.destroyBuffer(buffer, allocation);
}
} // namespace vee