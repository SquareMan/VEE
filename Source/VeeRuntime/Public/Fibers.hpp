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

#include "FiberContext.h"
#include "Name.hpp"


namespace vee {

/**
 * A fiber is a version of a lightweight userspace thread. It is cooperatively scheduled (fibers
 * switch to other fibers) and has its own stack.
 */
struct Fiber {
    Name name;
    void* stack = nullptr;
    /**
     * Platform/Architecture dependent context. Holds CPU registers for context-switching
     */
    FiberContext context;
};
/**
 * Retrieve a pointer to the currently executing fiber that called this function. May be nullptr if
 * not called by a fiber.
 */
Fiber* current_fiber();

/**
 * Context switch to another Fiber. Calling fiber will resume immediately after this call when it is
 * later switched to from another fiber
 * @note destination Fiber must NOT be already executing.
 * @param destination Fiber to begin executing. If the fiber is to be executed for the first time,
 * it will begin at its entry point that was provided when it was created. If the fiber is suspended
 * it will continue from where it last left off.
 */
void switch_to_fiber(Fiber& destination);
/**
 * Create a new Fiber. The newly created fiber will be in a pending state and need to be scheduled
 * later with switch_to_fiber in order to execute it.
 * @param entry Pointer to the fiber's entry point to begin execution.
 * @param name A name to associate with the fiber. Used by the profiler.
 */
Fiber create_fiber(void (*entry)(), Name name);
/**
 * Destroy a Fiber. The fiber must not be currently executing.
 */
void destroy_fiber(Fiber& fiber);

/**
 * Convert the thread that called this function into a fiber.
 * @param fiber Reference to a new empty fiber struct.
 */
void convert_thread_to_fiber(Fiber& fiber);
/**
 * Convert the fiber that called this function back into a normal thread.
 */
void convert_fiber_to_thread();
} // namespace vee
