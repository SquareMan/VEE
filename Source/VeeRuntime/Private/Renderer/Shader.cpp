//
// Created by Square on 10/3/2024.
//

module;
#include <vector>
module Vee.Renderer;

import VkUtil;

namespace vee::vulkan {
Shader::Shader(vk::Device device, vk::ShaderStageFlagBits stage, const std::vector<char>& code) {
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