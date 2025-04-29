
#ifdef VEE_WITH_EDITOR
#include "EditorApplication.hpp"
#else
#include "Application.hpp"
#endif
#include "Renderer/RenderCtx.hpp"

int main(int argc, char* argv[]) {
    using namespace vee;

    auto window = platform::Window::create("Hello Triangle", 640, 640);
    if (!window.has_value()) {
        return 1;
    }
    platform::Window w = std::move(window.value());
#ifdef VEE_WITH_EDITOR
    entt::locator<IApplication>::reset(new EditorApplication(std::move(w)));
#else
    entt::locator<IApplication>::reset(new Application(std::move(w)));
#endif
    entt::locator<IApplication>::value().run();
    entt::locator<IApplication>::reset();
    RenderCtx::ShutdownService();

    return 0;
}