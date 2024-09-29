#include "VeeCore.hpp"


#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <cassert>
#include <iostream>
#include <GLFW/glfw3native.h>

#include "Renderer.hpp"


int main(int argc, char *argv[]) {
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *glfw_window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (!glfw_window) {
        glfwTerminate();
        return -1;
    }

    const Vee::Window window = glfwGetWin32Window(glfw_window);
    assert(window != nullptr);

    {
        Vee::Renderer renderer(window);

        while (!glfwWindowShouldClose(glfw_window)) {
            renderer.Render();

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    glfwTerminate();

    std::cout << "Goodbye\n";

    return 0;
}
