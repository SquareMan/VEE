//
// Created by Square on 9/28/2024.
//

#pragma once

#include "Renderer/Buffer.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/IRenderer.hpp"
#include "Renderer/Pipeline.hpp"

#include <functional>

namespace vee {
class RenderCtx;
struct Vertex;
class Renderer final {
public:
    explicit Renderer();
    ~Renderer();
    void init();

    void Render();

    template <class T>
    void push_renderer() {
        static_assert(std::is_base_of_v<IRenderer, T>, "T must derive from IRenderer");
        auto v = renderers_.emplace_back(std::make_shared<T>());
    }

private:
    void record_commands(vk::CommandBuffer cmd, const std::function<void(vk::CommandBuffer cmd)>& func);

    std::vector<std::shared_ptr<IRenderer>> renderers_;
};
} // namespace vee