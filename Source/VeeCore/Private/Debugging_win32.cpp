//
// Created by Square on 7/5/2025.
//

#include "Debugging.hpp"

#include <Windows.h>


bool is_debugger_attached() {
    return IsDebuggerPresent();
}
