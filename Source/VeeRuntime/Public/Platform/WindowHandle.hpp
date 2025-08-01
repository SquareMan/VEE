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
