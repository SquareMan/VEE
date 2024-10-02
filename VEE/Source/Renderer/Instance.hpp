//
// Created by Square on 10/1/2024.
//
#pragma once

#include <volk/volk.h>
#include <string>
#include <vector>

namespace Vee::Vulkan {
class Instance final {
public:
    Instance(
        VkInstance vk_instance,
        std::vector<std::string>&& enabled_layers,
        std::vector<std::string>&& enabled_extensions
    );

    VkInstance vk_instance;
private:
    std::vector<std::string> enabled_layers;
    std::vector<std::string> enabled_extensions;
};

class InstanceBuilder final {
public:
    Instance build();
    InstanceBuilder& with_application_name(const char* application_name);
    InstanceBuilder& with_application_version(uint32_t application_version);
    InstanceBuilder& with_layers(const std::vector<const char*>& requested_layer_names);
    InstanceBuilder& with_extensions(const std::vector<const char*>& requested_extension_names);

private:
    const char* m_application_name = nullptr;
    uint32_t m_application_version = 0;
    std::vector<const char*> m_requested_layer_names;
    std::vector<const char*> m_requested_extension_names;
};

} // namespace Vee::Vulkan
