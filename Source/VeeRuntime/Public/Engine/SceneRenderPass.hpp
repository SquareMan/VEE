//
// Created by Square on 6/8/2025.
//

#pragma once

#include "RenderGraph/Pass.hpp"


#include <vulkan/vulkan.hpp>

namespace vee {
class Buffer;
}
namespace vee::rdg {
class ImageResource;
class SceneRenderPass : public Pass {
public:
    SceneRenderPass();

    void execute(vk::CommandBuffer cmd) override;

protected:
    std::shared_ptr<ImageResource> render_target_;
    std::shared_ptr<Buffer> vertex_buffer_;
    std::shared_ptr<Buffer> index_buffer_;
};
} // namespace vee::renderer
