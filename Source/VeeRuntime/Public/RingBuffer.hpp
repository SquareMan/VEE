//
// Created by Square on 10/2/2024.
//

#pragma once

#include <array>

namespace vee {
template <typename T, size_t I>
class RingBuffer {
public:
    T& operator[](size_t idx) {
        return buffer[idx];
    }

    [[nodiscard]] T& get_next() {
        if (index >= I) {
            index = 0;
        }
        return buffer[index++];
    }

    [[nodiscard]] size_t size() {
        return I;
    }

    std::array<T, I> buffer;
    uint32_t index = 0;
};
} // namespace vee