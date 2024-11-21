//
// Created by Square on 11/18/2024.
//

#pragma once

#include "Engine/Sprite.hpp"
#include "Renderer/Pipeline.hpp"

namespace vee {
class SpriteRendererComponent {
public:
    explicit SpriteRendererComponent(const Sprite& sprite);
    explicit SpriteRendererComponent(Sprite&& sprite);

public:
    vee::Sprite sprite_;
    vee::vulkan::Pipeline pipeline_;
    vk::DescriptorSet descriptor_set_;
};
}; // namespace vee