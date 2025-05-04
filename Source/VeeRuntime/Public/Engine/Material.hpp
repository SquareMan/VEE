//
// Created by Square on 5/4/2025.
//

#pragma once
#include "Renderer/Pipeline.hpp"


#include <expected>
#include <memory>

namespace vee {
class Texture;
class Material {
public:
    struct CreateError {};
    static std::expected<std::shared_ptr<Material>, CreateError> create(const std::shared_ptr<Texture>& texture);

protected:
    std::shared_ptr<Texture> texture_;
    vulkan::Pipeline pipeline_;
    vk::DescriptorSet descriptor_set_;

    friend class GameRenderer;
};
} // namespace vee
