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


#include "Engine/SceneRenderPass.hpp"

#include "Components/CameraComponent.hpp"
#include "Components/SpriteRendererComponent.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Material.hpp"
#include "IApplication.hpp"
#include "Renderer/Buffer.hpp"
#include "RenderGraph/DirectSource.hpp"
#include "RenderGraph/ImageResource.hpp"
#include "RenderGraph/Sink.hpp"
#include "Transform.h"

#include <entt/locator/locator.hpp>
#include <tracy/Tracy.hpp>

#ifdef VEE_WITH_EDITOR
#include <imgui.h>
#endif

namespace vee::rdg {
SceneRenderPass::SceneRenderPass() {
    register_source("render_target"_hash, DirectSource<ImageResource>::make(render_target_));
    register_source("vertex_buffer"_hash, DirectSource<Buffer>::make(vertex_buffer_));
    register_source("index_buffer"_hash, DirectSource<Buffer>::make(index_buffer_));
    register_sink("render_target"_hash, DirectSink<ImageResource>::make(render_target_));
}

void SceneRenderPass::execute(vk::CommandBuffer cmd) {
    ZoneScoped;

    Engine& engine = entt::locator<IApplication>::value().get_engine();

    // TODO: deal with multiple cameras
    auto cams = engine.get_world().entt_registry.view<vee::CameraComponent, vee::Transform>();
    auto [e, cam, cam_transform] = *cams.each().begin();

#ifdef VEE_WITH_EDITOR
    // FIXME: This does not belong in rendering code. It depends on the EditorRenderPass not being
    // rendered before this one.
    if (ImGui::Begin("Debug")) {
        ImGui::DragFloat2("Cam Pos", reinterpret_cast<float*>(&cam_transform.position));
        ImGui::DragFloat("Cam Rot", &cam_transform.rotation);
        ImGui::DragFloat2("Cam Scale", reinterpret_cast<float*>(&cam_transform.scale));
    }
    ImGui::End();
#endif

    const glm::mat4x4 proj = cam.calculate_view_projection(cam_transform);

    vk::ClearValue clear_value({0.3f, 0.77f, 0.5f, 1.0f});
    vk::RenderingAttachmentInfo render_attachment = {
        render_target_->view, vk::ImageLayout::eColorAttachmentOptimal, {}, {}, {}, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, clear_value
    };
    vk::RenderingInfo render_info = {{}, {{}, {render_target_->width, render_target_->height}}, 1, 0, render_attachment, {}, {}};

    cmd.beginRendering(render_info);
    {
        const vk::Rect2D scissor({}, {render_target_->width, render_target_->height});
        const vk::Viewport viewport(
            0, 0, static_cast<float>(render_target_->width), static_cast<float>(render_target_->height), 1.0f
        );

        cmd.setScissor(0, scissor);
        cmd.setViewport(0, viewport);

        auto view = engine.get_world().entt_registry.view<vee::Transform, vee::SpriteRendererComponent>(
        );
        for (const auto [ent, trans, spr] : view.each()) {
            std::shared_ptr<Material>& mat = spr.sprite_.material_;

            // Push view projection matrix
            // TODO: this should go in a buffer
            cmd.pushConstants(mat->pipeline_.layout, vk::ShaderStageFlagBits::eVertex, sizeof(glm::mat4x4), sizeof(glm::mat4x4), &proj);

            cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, mat->pipeline_.pipeline);

            glm::mat4x4 local_to_world = trans.to_mat();
            cmd.pushConstants(mat->pipeline_.layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4x4), &local_to_world);
            cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mat->pipeline_.layout, 0, mat->descriptor_set_, {});
            cmd.bindVertexBuffers(0, vertex_buffer_->buffer, {0});
            cmd.bindIndexBuffer(index_buffer_->buffer, 0, vk::IndexType::eUint16);
            cmd.drawIndexed(4, 1, 3, 0, 0);
        }
    }
    cmd.endRendering();
}
} // namespace vee::rdg