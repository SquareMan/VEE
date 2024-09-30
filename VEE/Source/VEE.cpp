#include <iostream>

#include "Renderer.hpp"


int main(int argc, char* argv[]) {
    const Vee::Platform::Window window(640, 640);
    Vee::Renderer renderer(window);

    while (!window.should_close()) {
        renderer.Render();
        window.poll_events();
    }

    std::cout << "Goodbye\n";

    return 0;
}