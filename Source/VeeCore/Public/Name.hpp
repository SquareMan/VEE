//
// Created by Square on 7/4/2025.
//

#pragma once

#include "FNV-1a.hpp"

#include <string>


namespace vee {
struct Name {
    size_t hash;

    Name() = default;
    Name(const Name&) = default;
    Name& operator=(const Name&) = default;
    Name(Name&&) = default;
    Name& operator=(Name&&) = default;
    constexpr Name(const char* s)
        : hash(utils::fnv1a_from_cstr(s)) {}

    bool operator==(const Name& other) const {
        return hash == other.hash;
    }
};
} // namespace vee

template <>
struct std::hash<vee::Name> {
    size_t operator()(const vee::Name handle) const noexcept {
        return handle.hash;
    }
};
