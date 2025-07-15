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

#include <bit>
#include <cstdint>

namespace vee::utils {

/**
 * Simple compile-time FNNv-1a implementation.
 * @param str String to be hashed.
 * @return Hash for str
 */
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

} // namespace vee::utils
