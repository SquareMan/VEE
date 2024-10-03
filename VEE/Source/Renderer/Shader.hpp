//
// Created by Square on 10/3/2024.
//

#pragma once

#define VK_NO_PROTOTYPES
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Vee::Vulkan {

class Shader final {
public:
    Shader(VkDevice device, VkShaderStageFlagBits stage, const std::vector<char>& code);
    ~Shader();

    [[nodiscard]] const char* entrypoint() const;

    VkShaderModule m_module = VK_NULL_HANDLE;
    VkShaderStageFlags m_stage = 0;
    std::string m_entrypoint = "main";

private:
    VkDevice m_device = VK_NULL_HANDLE;
};
} // namespace Vee::Vulkan