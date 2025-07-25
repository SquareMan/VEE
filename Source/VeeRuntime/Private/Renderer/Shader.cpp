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


#include "Renderer/Shader.hpp"


#include <vector>

namespace vee::vulkan {
Shader::Shader(vk::Device device, vk::ShaderStageFlagBits stage, const std::vector<std::byte>& code) {
    m_device = device;
    m_stage = stage;

    const vk::ShaderModuleCreateInfo shader_info({}, code.size(), reinterpret_cast<const uint32_t*>(code.data()));
    m_module = device.createShaderModule(shader_info).value;
}

Shader::~Shader() {
    m_device.destroyShaderModule(m_module);
}
const char* Shader::entrypoint() const {
    return m_entrypoint.c_str();
}
} // namespace vee::vulkan