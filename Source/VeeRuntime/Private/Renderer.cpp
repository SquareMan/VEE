//    Copyright 2025 Steven Casper
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.


#include "Renderer.hpp"

#include "Platform/Window.hpp"
#include "Renderer/RenderCtx.hpp"
#include "RenderGraph/RenderGraph.hpp"

#include <tracy/Tracy.hpp>


namespace vee {
Renderer::Renderer(const platform::Window& window)
    : render_ctx_(window) {}

Renderer::~Renderer() {
    // TODO: Cleanup everything
    std::ignore = render_ctx_.device.waitIdle();

#if _DEBUG
    static_cast<vk::Instance>(render_ctx_.instance).destroyDebugUtilsMessengerEXT(render_ctx_.debug_messenger_);
#endif

    for (CmdBuffer& command_buffer : render_ctx_.command_buffers.buffer) {
        render_ctx_.device.destroyFence(command_buffer.fence);
        render_ctx_.device.destroySemaphore(command_buffer.acquire_semaphore);
        render_ctx_.device.destroySemaphore(command_buffer.submit_semaphore);
    }
    render_ctx_.device.destroyCommandPool(render_ctx_.command_pool);
}

std::uint64_t Renderer::get_frame_number() const {
    return frame_num_;
}

RenderCtx& Renderer::get_ctx() {
    return render_ctx_;
}

void Renderer::render() {
    ZoneScoped;
    frame_num_++;

    if (render_graph_) {
        render_graph_->execute(render_ctx_);
    }
}

void Renderer::set_render_graph(std::unique_ptr<rdg::RenderGraph>&& render_graph) {
    render_graph_ = std::move(render_graph);
}
} // namespace vee