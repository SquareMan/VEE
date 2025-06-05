//
// Created by Square on 9/29/2024.
//

#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

namespace vee::platform {
using WindowHandle = HWND;
}
#endif
