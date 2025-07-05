
#ifdef VEE_WITH_EDITOR
#include "EditorApplication.hpp"
#include "EditorRenderPass.hpp"
#else
#include "Application.hpp"
#endif

#include <Engine/SceneRenderPass.hpp>
#include <RenderGraph/RenderGraph.hpp>
#include <RenderGraph/RenderGraphBuilder.hpp>

#if defined(TRACY_ENABLE) && !defined(TRACY_NO_FRAME_IMAGE)
#include <Engine/FrameImagePass.hpp>
#endif

int main() {
    using namespace vee;

    rdg::RenderGraphBuilder rg;

    rg.add_pass<rdg::SceneRenderPass>("scene")
        .link_sink({rdg::GLOBAL, "framebuffer"}, "render_target")
        .link_sink({rdg::GLOBAL, "vertex_buffer"}, "vertex_buffer")
        .link_sink({rdg::GLOBAL, "index_buffer"}, "index_buffer");
#ifdef VEE_WITH_EDITOR
    rg.add_pass<rdg::EditorRenderPass>("editor").link_sink({"scene", "render_target"}, "render_target");
    constexpr rdg::PassHandle frame_image_prev = "editor";
#else
    constexpr rdg::PassHandle frame_image_prev = "scene";
#endif
#if defined(TRACY_ENABLE) && !defined(TRACY_NO_FRAME_IMAGE)
    rg.add_pass<rdg::FrameImageRenderPass>("frame_image").link_sink({frame_image_prev, "render_target"}, "copy_source");
#endif

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

    auto& renderer = entt::locator<IApplication>::value().get_renderer();
    auto graph = std::make_unique<rdg::RenderGraph>(std::move(rg.build(renderer.get_ctx())));
    renderer.set_render_graph(std::move(graph));
    entt::locator<IApplication>::value().run();
    entt::locator<IApplication>::reset();

    return 0;
}