
#include "EditorApplication.hpp"

int main(int argc, char* argv[]) {
    const vee::platform::Window window("Hello Triangle", 640, 640);
    vee::EditorApplication::InitService(window);
    vee::EditorApplication::GetService().run();

    return 0;
}