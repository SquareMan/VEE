//
// Created by Square on 9/29/2024.
//

#pragma once

#ifdef WIN32
#include "Win32/WindowHandle.hpp"
#endif
#include "VeeCore.hpp"

#include "GLFW/glfw3.h"

#include <tuple>

namespace vee::platform {
class Window {
public:
    Window(const char* title, int32_t width, int32_t height);

    ~Window();

    void poll_events() const;
    bool should_close() const;

    VEE_NODISCARD std::tuple<uint32_t, uint32_t> get_size() const;

    VEE_NODISCARD WindowHandle get_handle() const;

    GLFWwindow* glfw_window = nullptr;
};
} // namespace Vee::Platform