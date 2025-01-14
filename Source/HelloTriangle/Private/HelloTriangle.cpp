
#include "Platform/Window.hpp"
#include <entt/entt.hpp>

import Vee.Engine;
import Vee.Editor;

int main(int argc, char* argv[]) {
    const vee::platform::Window window("Hello Triangle", 640, 640);
    vee::Application::InitService<vee::EditorApplication>(window);
    vee::Application::GetService().run();

    return 0;
}