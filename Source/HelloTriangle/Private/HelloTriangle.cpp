
#include "EditorApplication.hpp"
#include "Renderer/RenderCtx.hpp"

int main(int argc, char* argv[]) {
    const vee::platform::Window window("Hello Triangle", 640, 640);
    vee::EditorApplication::InitService(window);
    vee::EditorApplication::GetService().run();

    vee::EditorApplication::ShutdownService();
    vee::RenderCtx::ShutdownService();

    return 0;
}