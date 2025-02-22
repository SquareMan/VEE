//
// Created by Square on 9/29/2024.
//

#pragma once

#include "Keys.hpp"

#ifdef WIN32
#include "WindowHandle.hpp"
#endif
#include "VeeCore.hpp"

#include "GLFW/glfw3.h"

#include <set>
#include <tuple>

namespace vee::platform {
class Window {
public:
    Window(const char* title, int32_t width, int32_t height);

    ~Window();

    void poll_events() const;
    bool should_close() const;

    [[nodiscard]] std::tuple<uint32_t, uint32_t> get_size() const;

    [[nodiscard]] WindowHandle get_handle() const;

    GLFWwindow* glfw_window = nullptr;

    [[nodiscard]] bool is_key_down(Key key) const;

private:
    static void key_callback_dispatcher(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
    void key_callback(int key, int scancode, int action, int mods);

    std::set<Key> down_keys;
};
} // namespace vee::platform