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


/**
 * Determine if a debugger is currently attached to the game.
 * @note On Windows this will simply call IsDebuggerPresent
 * @note On Linux this will parse the "/proc/self/status" file and look for a non-zero TracerPid
 * @return True when game is being debugged.
 */
bool is_debugger_attached();

#ifndef VEE_DEBUG

/**
 * Set a programmatic breakpoint. Breaks into the debugger if one is attached.
 * @note These will be removed in non-debug builds
 */
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
