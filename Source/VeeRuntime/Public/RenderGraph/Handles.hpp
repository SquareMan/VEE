//
// Created by Square on 6/14/2025.
//

#pragma once

#include <Name.hpp>


// For now, these just need to be a precomputed hash, and that's exactly what Name is.

namespace vee::rdg {
using PassHandle = Name;
using SourceHandle = Name;
using SinkHandle = Name;

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
} // namespace vee::rdg
