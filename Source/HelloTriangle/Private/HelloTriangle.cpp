
#include "EditorApplication.hpp"
#include "GameRenderer.hpp"

int main(int argc, char* argv[]) {
    const vee::platform::Window window("Hello Triangle", 640, 640);
    auto Application = vee::EditorApplication(window);
    Application.get_renderer().push_renderer<ht::GameRenderer>();
    Application.run();

    return 0;
}