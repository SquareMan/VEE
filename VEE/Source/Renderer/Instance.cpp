//
// Created by Square on 10/1/2024.
//

#include "Instance.hpp"

#include "VkUtil.hpp"

#include <algorithm>
#include <iostream>
#include <vulkan/vk_enum_string_helper.h>

namespace Vee::Vulkan {
Instance::Instance(
    VkInstance vk_instance,
    std::vector<std::string>&& enabled_layers,
    std::vector<std::string>&& enabled_extensions
)
    : vk_instance(vk_instance)
    , enabled_layers(enabled_layers)
    , enabled_extensions(enabled_extensions) {}

Instance InstanceBuilder::build() {
    uint32_t num_layers = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&num_layers, nullptr))
    std::vector<VkLayerProperties> available_layers(num_layers);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&num_layers, available_layers.data()))

    std::vector<const char*> available_layer_names;
    std::ranges::transform(
        available_layers,
        std::back_inserter(available_layer_names),
        [](const VkLayerProperties& layer) { return layer.layerName; }
    );

    uint32_t num_extensions = 0;
    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &num_extensions, nullptr))
    std::vector<VkExtensionProperties> available_extensions(num_extensions);
    VK_CHECK(vkEnumerateInstanceExtensionProperties(
        nullptr, &num_extensions, available_extensions.data()
    ))

    std::vector<const char*> available_extension_names;
    std::ranges::transform(
        available_extensions,
        std::back_inserter(available_extension_names),
        [](const VkExtensionProperties& extension) { return extension.extensionName; }
    );

    const VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = m_application_name,
        .applicationVersion = m_application_version,
        .pEngineName = "VEE",
        .apiVersion = VK_API_VERSION_1_3,
    };

    std::vector<const char*> enabled_instance_layers =
        filter_extensions(available_layer_names, m_requested_layer_names);
    std::vector<const char*> enabled_instance_extensions =
        filter_extensions(available_extension_names, m_requested_extension_names);
    const VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = static_cast<uint32_t>(enabled_instance_layers.size()),
        .ppEnabledLayerNames = enabled_instance_layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(enabled_instance_extensions.size()),
        .ppEnabledExtensionNames = enabled_instance_extensions.data(),
    };

    VkInstance instance = VK_NULL_HANDLE;
    VK_CHECK(vkCreateInstance(&create_info, nullptr, &instance))
    volkLoadInstance(instance);


    std::vector<std::string> enabled_layers;
    std::vector<std::string> enabled_extensions;

    std::ranges::transform(
        enabled_instance_layers,
        std::back_inserter(enabled_layers),
        [](const char* name) { return name; }
    );
    std::ranges::transform(
        enabled_instance_extensions,
        std::back_inserter(enabled_extensions),
        [](const char* name) { return name; }
    );
    return {instance, std::move(enabled_layers), std::move(enabled_extensions)};
}
InstanceBuilder& InstanceBuilder::with_application_name(const char* application_name) {
    m_application_name = application_name;
    return *this;
}
InstanceBuilder& InstanceBuilder::with_application_version(uint32_t application_version) {
    m_application_version = application_version;
    return *this;
}
InstanceBuilder& InstanceBuilder::with_layers(const std::vector<const char*>& requested_layer_names
) {
    m_requested_layer_names.insert(
        m_requested_layer_names.end(), requested_layer_names.begin(), requested_layer_names.end()
    );
    return *this;
}
InstanceBuilder&
InstanceBuilder::with_extensions(const std::vector<const char*>& requested_extension_names) {
    m_requested_extension_names.insert(
        m_requested_extension_names.end(),
        requested_extension_names.begin(),
        requested_extension_names.end()
    );
    return *this;
}

} // namespace Vee::Vulkan