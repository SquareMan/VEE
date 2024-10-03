//
// Created by Square on 10/2/2024.
//

#pragma once

#include <array>

namespace Vee {
template <typename T, size_t I> class RingBuffer {
public:
    T& get_next() {
        if (index >= I) {
            index = 0;
        }
        return buffer[index++];
    }

    std::array<T, I> buffer;
    uint32_t index = 0;
};
} // namespace Vee