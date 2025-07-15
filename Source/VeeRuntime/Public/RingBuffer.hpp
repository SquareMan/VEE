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

#include <array>
#include <cstdint>

namespace vee {
template <typename T, std::size_t I>
class RingBuffer {
public:
    T& operator[](std::size_t idx) {
        return buffer[idx];
    }

    [[nodiscard]] T& get_next() {
        if (index >= I) {
            index = 0;
        }
        return buffer[index++];
    }

    [[nodiscard]] std::size_t size() {
        return I;
    }

    std::array<T, I> buffer;
    std::uint32_t index = 0;
};
} // namespace vee