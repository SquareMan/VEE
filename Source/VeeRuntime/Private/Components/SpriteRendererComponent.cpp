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