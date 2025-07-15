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
} // namespace vee::rdg
