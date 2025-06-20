//
// Created by Square on 6/14/2025.
//

#pragma once

#include <bit>
#include <string>

// Simple FNNv-1a implementation
constexpr std::size_t hash_string(const char* str) {
    constexpr std::uint64_t FNV_OFFSET_BASIS = 14695981039346656037u;
    constexpr std::uint64_t FNV_PRIME = 1099511628211u;

    std::uint64_t hash = FNV_OFFSET_BASIS;
    for (; *str != '\0'; ++str) {
        hash ^= *str;
        hash *= FNV_PRIME;
    }

    return std::bit_cast<std::size_t>(hash);
}

struct ResourceHandle {
    size_t hash;

    ResourceHandle() = default;
    ResourceHandle(const ResourceHandle&) = default;
    ResourceHandle& operator=(const ResourceHandle&) = default;
    ResourceHandle(ResourceHandle&&) = default;
    ResourceHandle& operator=(ResourceHandle&&) = default;
    constexpr ResourceHandle(const char* s)
        : hash(hash_string(s)) {}

    bool operator==(const ResourceHandle& other) const {
        return hash == other.hash;
    }
};

template <>
struct std::hash<ResourceHandle> {
    size_t operator()(const ResourceHandle handle) const noexcept {
        return handle.hash;
    }
};

// For now, these just need to be a precomputed hash, and that's exactly what ResourceHandle is.

using PassHandle = ResourceHandle;
using SourceHandle = ResourceHandle;
using SinkHandle = ResourceHandle;

/**
 * A Graph-Scoped reference to a Sink.
 */
struct SinkRef {
    /**
     * Handle to the Pass owning the referenced Sink
     */
    PassHandle pass;
    /**
     * Handle to the referenced Sink
     */
    SinkHandle sink;
};
