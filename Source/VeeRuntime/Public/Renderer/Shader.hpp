//
// Created by Square on 10/3/2024.
//

#pragma once

#include <string>
#include <vector>

import vulkan_hpp;

namespace vee::vulkan {

class Shader final {
public:
    Shader(vk::Device device, vk::ShaderStageFlagBits stage, const std::vector<char>& code);
    ~Shader();

    [[nodiscard]] const char* entrypoint() const;

    vk::ShaderModule m_module;
    vk::ShaderStageFlagBits m_stage;
    std::string m_entrypoint = "main";

private:
    vk::Device m_device;
};
} // namespace vee::vulkan