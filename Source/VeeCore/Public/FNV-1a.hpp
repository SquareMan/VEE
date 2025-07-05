//
// Created by Square on 7/4/2025.
//

#pragma once

#include <bit>
#include <cstdint>

namespace vee::utils
{
  
// Simple FNNv-1a implementation
constexpr std::size_t fnv1a_from_cstr(const char* str) {
    static_assert(sizeof(std::size_t) == sizeof(std::uint64_t));
    constexpr std::uint64_t FNV_OFFSET_BASIS = 14695981039346656037u;
    constexpr std::uint64_t FNV_PRIME = 1099511628211u;

    std::uint64_t hash = FNV_OFFSET_BASIS;
    for (; *str != '\0'; ++str) {
        hash ^= *str;
        hash *= FNV_PRIME;
    }

    return std::bit_cast<std::size_t>(hash);
}

}
