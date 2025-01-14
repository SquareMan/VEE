//
// Created by Square on 11/18/2024.
//

module;

#include <compare>
#include <tuple>
#include <xmemory>
#include <vector>

#include "Engine/Sprite.hpp"
#include "Renderer/Pipeline.hpp"

export module SpriteRendererComponent;

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