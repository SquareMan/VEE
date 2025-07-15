//
// Created by Square on 7/13/2025.
//

#pragma once

#if defined(_WIN32)
#include "FiberContext_win32_x64.h"
#elif defined(__linux)
#include "FiberContext_linux_x64.h"
#endif
