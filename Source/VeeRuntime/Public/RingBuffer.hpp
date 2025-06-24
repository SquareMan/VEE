//
// Created by Square on 10/2/2024.
//

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