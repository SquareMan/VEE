//
// Created by Square on 10/3/2024.
//

#include "Shader.hpp"

#include "VkUtil.hpp"

#include <iostream>
#include <vector>
#include <volk/volk.h>
#include <vulkan/vk_enum_string_helper.h>

namespace Vee::Vulkan {
Shader::Shader(VkDevice device, VkShaderStageFlagBits stage, const std::vector<char>& code) {
    m_device = device;
    m_stage = stage;

    VkShaderModuleCreateInfo shader_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const uint32_t*>(code.data())
    };
    VK_CHECK(vkCreateShaderModule(device, &shader_info, nullptr, &m_module));
}

Shader::~Shader() {
    vkDestroyShaderModule(m_device, m_module, nullptr);
}
const char* Shader::entrypoint() const {
    return m_entrypoint.c_str();
}
} // namespace Vee::Vulkan