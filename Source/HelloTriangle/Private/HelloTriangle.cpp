
#include "EditorApplication.hpp"
#include "Renderer/RenderCtx.hpp"

int main(int argc, char* argv[]) {
    auto window = vee::platform::Window::create("Hello Triangle", 640, 640);
    if (!window.has_value()) {
        return 1;
    }
    vee::platform::Window w = std::move(std::move(window.value()));
    vee::EditorApplication::InitService(std::move(w));
    vee::EditorApplication::GetService().run();

    vee::EditorApplication::ShutdownService();
    vee::RenderCtx::ShutdownService();

    return 0;
}