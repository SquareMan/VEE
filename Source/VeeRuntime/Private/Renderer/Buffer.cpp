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