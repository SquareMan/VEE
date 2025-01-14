//
// Created by Square on 10/3/2024.
//

module;
#include <string>
#include <vector>
export module Vee.Renderer:Shader;

import vulkan_hpp;

namespace vee::vulkan {

export class Shader final {
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