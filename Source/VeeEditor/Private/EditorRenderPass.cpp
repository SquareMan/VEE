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


#include "EditorRenderPass.hpp"

#include "Renderer.hpp"
#include "RenderGraph/DirectSource.hpp"
#include "RenderGraph/ImageResource.hpp"

#include <backends/imgui_impl_vulkan.h>
#include <entt/locator/locator.hpp>
#include <imgui.h>
#include <tracy/Tracy.hpp>

vee::rdg::EditorRenderPass::EditorRenderPass() {
    register_source("render_target"_hash, DirectSource<ImageResource>::make(render_target_));
    register_sink("render_target"_hash, DirectSink<ImageResource>::make(render_target_));
}

void vee::rdg::EditorRenderPass::execute(vk::CommandBuffer cmd) {
    ZoneScoped;

    ImGui::Render();
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    vk::RenderingAttachmentInfo render_attachment = {
        render_target_->view,
        vk::ImageLayout::eColorAttachmentOptimal,
        {},
        {},
        {},
        vk::AttachmentLoadOp::eLoad,
        vk::AttachmentStoreOp::eStore,
    };
    vk::RenderingInfo render_info = {{}, {{}, {render_target_->width, render_target_->height}}, 1, 0, render_attachment, {}, {}};
    cmd.beginRendering(render_info);
    { ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd); }
    cmd.endRendering();
}