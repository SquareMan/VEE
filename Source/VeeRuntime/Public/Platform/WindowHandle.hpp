//
// Created by Square on 9/29/2024.
//

#pragma once

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

namespace vee::platform {
using WindowHandle = HWND;
}
#elif defined(__linux)
#include <X11/Xlib.h>
struct WindowHandle {
    Display* display;
    Window window;
};
#endif
