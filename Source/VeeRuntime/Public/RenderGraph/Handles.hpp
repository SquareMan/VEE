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
