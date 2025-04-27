//
// Created by Square on 9/29/2024.
//

#include "Platform/Window.hpp"

#include "Assert.hpp"
#include <functional>
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include <magic_enum/magic_enum.hpp>

namespace vee::platform {
Window::Window(GLFWwindow& glfw_window)
    : glfw_window(&glfw_window) {
    glfwSetWindowUserPointer(&glfw_window, this);
    glfwSetKeyCallback(&glfw_window, &Window::key_callback_dispatcher);
}

std::expected<Window, Window::CreateError> Window::create(const char* title, int32_t width, int32_t height) {
    if (!glfwInit()) {
        VASSERT(false, "Failed to initialize GLFW");
        return std::unexpected(Window::CreateError());
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* glfw_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!glfw_window) {
        VASSERT(false, "failed to create GLFW window");
        return std::unexpected(Window::CreateError());
    }

    return std::move(Window(*glfw_window));
}

Window::~Window() {
    if (glfw_window != nullptr) {
        glfwDestroyWindow(glfw_window);
        glfwTerminate();
    }
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

bool Window::is_key_down(Key key) const {
    return down_keys.contains(key);
}

void Window::key_callback_dispatcher(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
    void* window = glfwGetWindowUserPointer(glfw_window);
    VASSERT(window != nullptr, "GLFW Window user data should contain a pointer to the vee::platform::Window class that owns it.");

    Window* window_ptr = static_cast<Window*>(window);
    window_ptr->key_callback(key, scancode, action, mods);
}

static bool key_valid(Key key) {
    switch (key) {
    case Key::Unknown:
    case Key::Space:
    case Key::Apostrophe:
    case Key::Comma:
    case Key::Minus:
    case Key::Period:
    case Key::Slash:
    case Key::Num0:
    case Key::Num1:
    case Key::Num2:
    case Key::Num3:
    case Key::Num4:
    case Key::Num5:
    case Key::Num6:
    case Key::Num7:
    case Key::Num8:
    case Key::Num9:
    case Key::Semicolon:
    case Key::Equal:
    case Key::A:
    case Key::B:
    case Key::C:
    case Key::D:
    case Key::E:
    case Key::F:
    case Key::G:
    case Key::H:
    case Key::I:
    case Key::J:
    case Key::K:
    case Key::L:
    case Key::M:
    case Key::N:
    case Key::O:
    case Key::P:
    case Key::Q:
    case Key::R:
    case Key::S:
    case Key::T:
    case Key::U:
    case Key::V:
    case Key::W:
    case Key::X:
    case Key::Y:
    case Key::Z:
    case Key::Left_Bracket:
    case Key::Backslash:
    case Key::Right_Bracket:
    case Key::Grave_Accent:
    case Key::World_1:
    case Key::World_2:
    case Key::Escape:
    case Key::Enter:
    case Key::Tab:
    case Key::Backspace:
    case Key::Insert:
    case Key::Delete:
    case Key::Right:
    case Key::Left:
    case Key::Down:
    case Key::Up:
    case Key::Page_Up:
    case Key::Page_Down:
    case Key::Home:
    case Key::End:
    case Key::Caps_Lock:
    case Key::Scroll_Lock:
    case Key::Num_Lock:
    case Key::Print_Screen:
    case Key::Pause:
    case Key::F1:
    case Key::F2:
    case Key::F3:
    case Key::F4:
    case Key::F5:
    case Key::F6:
    case Key::F7:
    case Key::F8:
    case Key::F9:
    case Key::F10:
    case Key::F11:
    case Key::F12:
    case Key::F13:
    case Key::F14:
    case Key::F15:
    case Key::F16:
    case Key::F17:
    case Key::F18:
    case Key::F19:
    case Key::F20:
    case Key::F21:
    case Key::F22:
    case Key::F23:
    case Key::F24:
    case Key::F25:
    case Key::Kp_0:
    case Key::Kp_1:
    case Key::Kp_2:
    case Key::Kp_3:
    case Key::Kp_4:
    case Key::Kp_5:
    case Key::Kp_6:
    case Key::Kp_7:
    case Key::Kp_8:
    case Key::Kp_9:
    case Key::Kp_Decimal:
    case Key::Kp_Divide:
    case Key::Kp_Multiply:
    case Key::Kp_Subtract:
    case Key::Kp_Add:
    case Key::Kp_Enter:
    case Key::Kp_Equal:
    case Key::Left_Shift:
    case Key::Left_Control:
    case Key::Left_Alt:
    case Key::Left_Super:
    case Key::Right_Shift:
    case Key::Right_Control:
    case Key::Right_Alt:
    case Key::Right_Super:
    case Key::Menu:
        return true;
    }
    return false;
}

void Window::key_callback(int key, int scancode, int action, int mods) {
    auto vkey = static_cast<Key>(key);
    VASSERT(key_valid(vkey), "An invalid key code was pressed.");
    log_trace(
        "Input Event: Key {}({}, {}) {} (Mods: {})",
        magic_enum::enum_name<Key>(vkey),
        key,
        scancode,
        action == GLFW_PRESS ? "Down"
        : action == GLFW_RELEASE
            ? "Up"
            : "Repeat",
        mods
    );
    if (action == GLFW_RELEASE) {
        down_keys.erase(vkey);
    } else {
        down_keys.insert(vkey);
    }
}
} // namespace vee::platform
