//
// Created by Square on 11/18/2024.
//

#include "Components/SpriteRendererComponent.hpp"

#include "Assert.hpp"
#include "IApplication.hpp"
#include "Platform/Filesystem.hpp"
#include "Renderer.hpp"
#include "Renderer/RenderCtx.hpp"
#include "Renderer/Shader.hpp"

#include <entt/locator/locator.hpp>

vee::SpriteRendererComponent::SpriteRendererComponent(const Sprite& sprite)
    : sprite_(sprite) {}

vee::SpriteRendererComponent::SpriteRendererComponent(Sprite&& sprite)
    : sprite_(sprite) {}