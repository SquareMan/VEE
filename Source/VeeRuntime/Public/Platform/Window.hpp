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

#include "Keys.hpp"

#include "WindowHandle.hpp"

#include "GLFW/glfw3.h"

#include <expected>
#include <set>
#include <tuple>

namespace vee::platform {

class Window {
public:
    Window(Window& other) = delete;
    Window& operator=(Window& other) = delete;

    Window(Window&& other) {
        *this = std::move(other);
        glfwSetWindowUserPointer(glfw_window, this);
    };
    Window& operator=(Window&& other) {
        this->glfw_window = other.glfw_window;
        other.glfw_window = nullptr;
        return *this;
    }


    explicit Window(GLFWwindow& glfw_window);

    struct CreateError {};
    static std::expected<Window, CreateError> create(const char* title, int32_t width, int32_t height);

    ~Window();

    void poll_events();
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