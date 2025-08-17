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

#include <vulkan/vulkan.hpp>

#include <string>

namespace vee::vulkan {

class Shader final {
public:
    Shader(vk::Device device, vk::ShaderStageFlagBits stage, const std::span<const std::uint32_t>& code, std::string&& entry_point = "main");
    ~Shader();

    [[nodiscard]] const char* entrypoint() const;
    [[nodiscard]] vk::ShaderModule module() const;

    vk::ShaderStageFlagBits m_stage;
    std::string m_entrypoint = "main";

private:
    vk::ShaderModule m_module;
    vk::Device m_device;
};
} // namespace vee::vulkan