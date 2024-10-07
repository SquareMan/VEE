//
// Created by Square on 10/1/2024.
//

#include "Instance.hpp"

#include "VkUtil.hpp"

#include <algorithm>

namespace Vee::Vulkan {
Instance::Instance(
    vk::Instance vk_instance,
    std::vector<std::string>&& enabled_layers,
    std::vector<std::string>&& enabled_extensions
)
    : vk_instance(vk_instance)
    , enabled_layers(enabled_layers)
    , enabled_extensions(enabled_extensions) {}

bool Instance::is_extension_enabled(const char* test_extension) {
    return std::ranges::find_if(
               enabled_extensions,
               [test_extension](const std::string& extension) {
                   return strcmp(test_extension, extension.c_str()) == 0;
               }
           )
           != enabled_extensions.end();
}

Instance InstanceBuilder::build() {
    std::vector<vk::LayerProperties> available_layers =
        vk::enumerateInstanceLayerProperties().value;

    std::vector<const char*> available_layer_names;
    std::ranges::transform(
        available_layers,
        std::back_inserter(available_layer_names),
        [](const VkLayerProperties& layer) { return layer.layerName; }
    );

    std::vector<vk::ExtensionProperties> available_extensions =
        vk::enumerateInstanceExtensionProperties().value;

    std::vector<const char*> available_extension_names;
    std::ranges::transform(
        available_extensions,
        std::back_inserter(available_extension_names),
        [](const VkExtensionProperties& extension) { return extension.extensionName; }
    );

    const vk::ApplicationInfo app_info = {
        m_application_name, m_application_version, "VEE", VK_API_VERSION_1_3
    };

    std::vector<const char*> enabled_instance_layers =
        filter_extensions(available_layer_names, m_requested_layer_names);
    std::vector<const char*> enabled_instance_extensions =
        filter_extensions(available_extension_names, m_requested_extension_names);

    vk::Instance instance = vk::createInstance(
                                {
                                    {},
                                    &app_info,
                                    enabled_instance_layers,
                                    enabled_instance_extensions,
                                },
                                nullptr
    )
                                .value;
    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

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