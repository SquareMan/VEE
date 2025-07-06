#pragma once

#include "Debugging.hpp"

#ifdef NDEBUG

#define VEE_DEBUGBREAK()

#else

#if defined(_MSC_VER)
#define VEE_DEBUGBREAK()                                                                           \
    if (is_debugger_attached()) {                                                                  \
        __debugbreak();                                                                            \
    } else {                                                                                       \
    }
#elif defined(__clang__)
#define VEE_DEBUGBREAK()                                                                           \
    if (is_debugger_attached()) {                                                                  \
        __builtin_debugtrap();                                                                     \
    } else {                                                                                       \
    }
#endif

#endif
