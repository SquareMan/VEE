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


#include "Fibers.hpp"
#include "Assert.hpp"
#include "Logging.hpp"

#include <tracy/Tracy.hpp>


namespace vee {

extern void fiber_context_switch(FiberContext* from, const FiberContext* to);

thread_local Fiber* t_current_fiber;
Fiber* current_fiber() {
    return t_current_fiber;
}

void switch_to_fiber(Fiber& destination) {
    TracyFiberEnter(destination.name.to_string().data());
    Fiber* from = current_fiber();
    t_current_fiber = &destination;
    fiber_context_switch(&from->context, &destination.context);
    TracyFiberLeave;
}

Fiber create_fiber(void (*entry)(), Name name) {
    std::size_t size = 512 * 1024;
    void* stack = malloc(size);

    const auto stack_top = reinterpret_cast<uintptr_t*>(static_cast<std::byte*>(stack) + size);
    return {name, stack, {.rip = std::bit_cast<uintptr_t>(entry), .rsp = std::bit_cast<uintptr_t>(&stack_top[-1])}};
}

void destroy_fiber(Fiber& fiber) {
    free(fiber.stack);
}

void convert_thread_to_fiber(Fiber& fiber) {
    VASSERT(current_fiber() == nullptr, "Thread has already been converted to a fiber");
    VASSERT(fiber.stack == nullptr, "A thread cannot be converted to an existing fiber");
    t_current_fiber = &fiber;
}

void convert_fiber_to_thread() {
    VASSERT(current_fiber() != nullptr);
    // Note: this is owned externally
    t_current_fiber = nullptr;
}
} // namespace vee