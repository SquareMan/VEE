
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

    rg.add_pass<rdg::SceneRenderPass>("scene"_hash)
        .link_sink({rdg::GLOBAL, "framebuffer"_hash}, "render_target"_hash)
        .link_sink({rdg::GLOBAL, "vertex_buffer"_hash}, "vertex_buffer"_hash)
        .link_sink({rdg::GLOBAL, "index_buffer"_hash}, "index_buffer"_hash);
#ifdef VEE_WITH_EDITOR
    rg.add_pass<rdg::EditorRenderPass>("editor"_hash).link_sink({"scene"_hash, "render_target"_hash}, "render_target"_hash);
    rdg::PassHandle frame_image_prev = "editor"_hash;
#else
    rdg::PassHandle frame_image_prev = "scene"_hash;
#endif
#if defined(TRACY_ENABLE) && !defined(TRACY_NO_FRAME_IMAGE)
    rg.add_pass<rdg::FrameImageRenderPass>("frame_image"_hash).link_sink({frame_image_prev, "render_target"_hash}, "copy_source"_hash);
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