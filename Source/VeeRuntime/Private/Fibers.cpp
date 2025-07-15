//
// Created by Square on 7/11/2025.
//

#include "Assert.hpp"
#include "Fibers.hpp"
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