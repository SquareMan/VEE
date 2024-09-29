//
// Created by Square on 9/29/2024.
//

#pragma once

#ifdef WIN32
#include "Win32/WindowHandle.hpp"
#endif
#include "VeeCore.hpp"

#include "GLFW/glfw3.h"

namespace Vee::Platform {
    class Window {
    public:
        Window(int32_t width, int32_t height);
        ~Window();

        void poll_events() const;
        bool should_close() const;

        VEE_NODISCARD WindowHandle get_handle() const;
    private:
        GLFWwindow* glfw_window = nullptr;
    };
}
