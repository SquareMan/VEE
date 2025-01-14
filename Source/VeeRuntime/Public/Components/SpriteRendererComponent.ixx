//
// Created by Square on 11/18/2024.
//

module;

#include <compare>
#include <tuple>
#include <xmemory>
#include <vector>

#include "Engine/Sprite.hpp"

export module SpriteRendererComponent;

import vulkan_hpp;
import Vee.Renderer;

namespace vee {
export class SpriteRendererComponent {
public:
    explicit SpriteRendererComponent(const Sprite& sprite);
    explicit SpriteRendererComponent(Sprite&& sprite);

public:
    Sprite sprite_;
    vulkan::Pipeline pipeline_;
    vk::DescriptorSet descriptor_set_;
};
}; // namespace vee