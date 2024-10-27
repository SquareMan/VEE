
#include "EditorApplication.hpp"

int main(int argc, char* argv[]) {
    const vee::platform::Window window("Hello Triangle", 640, 640);
    auto Application = vee::EditorApplication(window);
    Application.run();

    return 0;
}