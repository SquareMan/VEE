//
// Created by Square on 9/29/2024.
//

#include "Platform/Window.hpp"

#include "Assert.hpp"
#include "GLFW/glfw3.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

namespace vee::platform {
Window::Window(const char* title, int32_t width, int32_t height) {
    if (!glfwInit()) {
        VASSERT(false, "Failed to initialize GLFW");
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfw_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!glfw_window) {
        VASSERT(false, "failed to create GLFW window");
        return;
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
std::tuple<uint32_t, uint32_t> Window::get_size() const {
    std::tuple<uint32_t, uint32_t> result;
    glfwGetWindowSize(glfw_window, reinterpret_cast<int32_t*>(&std::get<0>(result)), reinterpret_cast<int32_t*>(&std::get<1>(result)));
    return result;
}

WindowHandle Window::get_handle() const {
    VASSERT(glfw_window != nullptr, "GLFW window was not initialized");
    // TODO: not platform independent
    return glfwGetWin32Window(glfw_window);
}
} // namespace vee::platform
