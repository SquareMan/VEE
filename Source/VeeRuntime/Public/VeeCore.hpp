#pragma once

#if defined(_MSC_VER)
#define VEE_DEBUGBREAK __debugbreak
#elif defined(__clang__)
#define VEE_DEBUGBREAK __builtin_debugtrap
#endif
