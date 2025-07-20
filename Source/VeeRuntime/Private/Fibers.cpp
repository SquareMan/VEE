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

constexpr static std::size_t FIBER_STACK_SIZE = 1024 * 512;

extern "C" uintptr_t _vee_read_rsp();
asm(R"(
_vee_read_rsp:
    mov %rsp, %rax
    ret
)");

namespace vee {

extern void fiber_context_switch(FiberContext* from, const FiberContext* to);

thread_local Fiber* t_current_fiber;
Fiber* current_fiber() {
    VASSERT(
        t_current_fiber == nullptr

            || t_current_fiber->stack == nullptr // FIXME: Right now fibers created from a thread
                                                 // have a null stackptr in the Fiber struct
            || (_vee_read_rsp() >= (uintptr_t)t_current_fiber->stack
                && _vee_read_rsp() < (uintptr_t)t_current_fiber->stack + FIBER_STACK_SIZE), // FIXME: The
                                                                                     // stack size
                                                                                     // shouldn't
                                                                                     // just be a
                                                                                     // magic number
                                                                                     // here.
        "The current execution context's stack pointer does not belong to t_current_fiber.\nt_current_fiber->stack: 0x{:X}\n%rsp: 0x{:X}",
        (uintptr_t)t_current_fiber->stack,
        _vee_read_rsp()
    );
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
    void* stack = malloc(FIBER_STACK_SIZE);

    const auto stack_top = reinterpret_cast<uintptr_t*>(static_cast<std::byte*>(stack) + FIBER_STACK_SIZE);
    return {name, stack, {.rip = std::bit_cast<uintptr_t>(entry), .rsp = std::bit_cast<uintptr_t>(&stack_top[-1])}};
}

void destroy_fiber(Fiber& fiber) {
    if (fiber.stack != nullptr) {
        free(fiber.stack);
        fiber.stack = nullptr;
    }
}

void convert_thread_to_fiber(Fiber& fiber) {
    VASSERT(current_fiber() == nullptr, "Thread has already been converted to a fiber");
    VASSERT(fiber.stack == nullptr, "A thread cannot be converted to an existing fiber");
    t_current_fiber = &fiber;
    // TracyFiberEnter(fiber.name.to_string().data());
}

void convert_fiber_to_thread() {
    VASSERT(current_fiber() != nullptr);
    // Note: this is owned externally
    t_current_fiber = nullptr;
    // TracyFiberLeave;
}
} // namespace vee