//
// Created by Square on 9/29/2024.
//

#include "Window.hpp"

#include <cassert>

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

namespace Vee::Platform {
    Window::Window(int32_t width, int32_t height) {
        if (!glfwInit()) {
            // return -1;
            return;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfw_window = glfwCreateWindow(width, height, "Hello World", nullptr, nullptr);
        if (!glfw_window) {
            glfwTerminate();
            // return -1;
        }
    }

    Window::~Window() {
        glfwTerminate();
    }

    void Window::poll_events() const {
        glfwPollEvents();
    }

    bool Window::should_close() const {
        return glfwWindowShouldClose(glfw_window);
    }

    WindowHandle Window::get_handle() const {
        assert(glfw_window != nullptr);
        // TODO: not platform independent
        return glfwGetWin32Window(glfw_window);
    }
}
