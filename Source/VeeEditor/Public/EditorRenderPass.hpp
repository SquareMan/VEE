//
// Created by Square on 6/8/2025.
//

#pragma once

#include "RenderGraph/Pass.hpp"


#include <vulkan/vulkan.hpp>


namespace vee::rdg {
class ImageResource;
class EditorRenderPass : public Pass {
public:
    EditorRenderPass();

    void execute(vk::CommandBuffer cmd) override;

protected:
    std::shared_ptr<ImageResource> render_target_;
};
}


