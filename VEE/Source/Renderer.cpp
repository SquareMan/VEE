//
// Created by Square on 9/28/2024.
//

#include "Renderer.hpp"

#include "VeeCore.hpp"

#define VK_USE_PLATFORM_WIN32_KHR
#define VOLK_IMPLEMENTATION
#include <Volk/volk.h>

#include <vulkan/vk_enum_string_helper.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "Platform/Filesystem.hpp"

#define VK_CHECK(func)                                                                             \
    {                                                                                              \
        const VkResult result = func;                                                              \
        if (result != VK_SUCCESS) {                                                                \
            std::cerr << "Error calling function " << #func << " at " << __FILE__ << ":"           \
                      << __LINE__ << ". Result is " << string_VkResult(result) << "\n";            \
            VEE_DEBUGBREAK();                                                                      \
        }                                                                                          \
    }

static VEE_NODISCARD std::vector<const char*> filter_extensions(
    std::vector<const char*>& available_extensions, std::vector<const char*>& requested_extensions
) {
    std::ranges::sort(available_extensions, [](const char* a, const char* b) {
        return std::strcmp(a, b) < 0;
    });
    std::ranges::sort(requested_extensions, [](const char* a, const char* b) {
        return std::strcmp(a, b) < 0;
    });

    std::vector<const char*> result;
    std::ranges::set_intersection(
        requested_extensions,
        available_extensions,
        std::back_inserter(result),
        [](const char* a, const char* b) { return std::strcmp(a, b) == 0; }
    );

    return result;
}


namespace Vee {
Renderer::Renderer(const Platform::Window& window) {
    VK_CHECK(volkInitialize())

    uint32_t num_layers = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&num_layers, nullptr))
    std::vector<VkLayerProperties> available_layers(num_layers);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&num_layers, available_layers.data()))

    std::vector<const char*> requested_layer_names = {
#if _DEBUG
        "VK_LAYER_KHRONOS_validation",
#endif
    };
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

#if !defined(VK_KHR_win32_surface) || !defined(VK_KHR_surface)
#error Renderer requires win32 platform and vulkan surface extensions
#endif

    std::vector<const char*> requested_extension_names = {
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(VK_EXT_debug_utils)
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
    };
    std::vector<const char*> available_extension_names;
    std::ranges::transform(
        available_extensions,
        std::back_inserter(available_extension_names),
        [](const VkExtensionProperties& extension) { return extension.extensionName; }
    );

    std::vector<const char*> enabled_instance_layers =
        filter_extensions(available_layer_names, requested_layer_names);
    std::vector<const char*> enabled_instance_extensions =
        filter_extensions(available_extension_names, requested_extension_names);

    if (std::ranges::find(enabled_instance_extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) !=
        enabled_instance_extensions.end()) {
        // TODO log error
        assert(false);
        return;
    }

    constexpr VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "VEE POC",
        .applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
        .pEngineName = "VEE",
        .apiVersion = VK_API_VERSION_1_3,
    };

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


    VkSurfaceKHR surface = VK_NULL_HANDLE;
    const VkWin32SurfaceCreateInfoKHR ci = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hinstance = GetModuleHandle(nullptr),
        .hwnd = window.get_handle(),
    };
    VK_CHECK(vkCreateWin32SurfaceKHR(instance, &ci, nullptr, &surface))

    uint32_t num_physical_devices = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &num_physical_devices, nullptr))
    std::vector<VkPhysicalDevice> physical_devices(num_physical_devices);
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &num_physical_devices, physical_devices.data()))

    assert(!physical_devices.empty());
    gpu = physical_devices[0];

    VkPhysicalDeviceProperties physical_properties;
    vkGetPhysicalDeviceProperties(gpu, &physical_properties);

    VkPhysicalDeviceFeatures physical_features;
    vkGetPhysicalDeviceFeatures(gpu, &physical_features);


    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, queue_families.data());

    uint32_t graphics_family_index = UINT32_MAX;
    uint32_t presentation_family_index = UINT32_MAX;

    for (uint32_t idx = 0; idx < queue_families.size(); ++idx) {
        if (graphics_family_index == UINT32_MAX &&
            queue_families[idx].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphics_family_index = idx;
        }

#if defined(VK_KHR_surface)
        if (presentation_family_index == UINT32_MAX && surface != VK_NULL_HANDLE) {
            VkBool32 supports_present = VK_FALSE;
            VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, idx, surface, &supports_present))
            if (supports_present == VK_TRUE) {
                presentation_family_index = idx;
            }
        }
#endif
    }

    uint32_t property_count = 0;
    VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu, nullptr, &property_count, nullptr))
    std::vector<VkExtensionProperties> properties(property_count);
    VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu, nullptr, &property_count, properties.data()))

    std::vector<const char*> available_property_names;
    std::ranges::transform(
        properties,
        std::back_inserter(available_property_names),
        [](const auto& property) { return property.extensionName; }
    );

    std::vector<const char*> requested_property_names = {"VK_KHR_swapchain"};
    std::vector<const char*> enabled_property_names =
        filter_extensions(available_property_names, requested_property_names);

    const float priority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> qcis = {
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = graphics_family_index,
            .queueCount = 1,
            .pQueuePriorities = &priority,
        },
        // {
        //     .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        //     .queueFamilyIndex = presentation_family_index,
        //     .queueCount = 1,
        //     .pQueuePriorities = &priority,
        // },
    };

    const VkDeviceCreateInfo dci = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<uint32_t>(qcis.size()),
        .pQueueCreateInfos = qcis.data(),
        .enabledExtensionCount = static_cast<uint32_t>(enabled_property_names.size()),
        .ppEnabledExtensionNames = enabled_property_names.data()
    };

    device = VK_NULL_HANDLE;
    vkCreateDevice(gpu, &dci, nullptr, &device);
    volkLoadDevice(device);

    queue = VK_NULL_HANDLE;
    vkGetDeviceQueue(device, graphics_family_index, 0, &queue);


    const VkCommandPoolCreateInfo cpci = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = graphics_family_index,
    };

    command_pool = VK_NULL_HANDLE;
    VK_CHECK(vkCreateCommandPool(device, &cpci, nullptr, &command_pool));


    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &format_count, nullptr);
    std::vector<VkSurfaceFormatKHR> surface_formats(format_count);
    VK_CHECK(
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &format_count, surface_formats.data())
    );

    VkFormat format = VK_FORMAT_UNDEFINED;
    for (const VkSurfaceFormatKHR& surface_format : surface_formats) {
        if (surface_format.format == VK_FORMAT_B8G8R8A8_SRGB) {
            format = surface_format.format;
            break;
        }
    }

    VkSurfaceCapabilitiesKHR surface_capabilities = {};
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &surface_capabilities));

    // swapchain
    uint32_t img_count = surface_capabilities.minImageCount + 1;
    img_count = img_count > surface_capabilities.maxImageCount ? img_count - 1 : img_count;

    swapchain = VK_NULL_HANDLE;
    const VkSwapchainCreateInfoKHR sci = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = img_count,
        .imageFormat = format,
        .imageExtent = surface_capabilities.currentExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .preTransform = surface_capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    };
    VK_CHECK(vkCreateSwapchainKHR(device, &sci, nullptr, &swapchain));

    uint32_t sc_img_count = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &sc_img_count, nullptr));
    swapchain_images.resize(sc_img_count);
    VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &sc_img_count, swapchain_images.data()));

    swapchain_image_views.resize(sc_img_count);
    VkImageViewCreateInfo image_view_info =
        {.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
         .viewType = VK_IMAGE_VIEW_TYPE_2D,
         .format = format,
         .subresourceRange = {
             .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
             .levelCount = 1,
             .layerCount = 1,
         }};
    for (uint32_t i = 0; i < sc_img_count; ++i) {
        image_view_info.image = swapchain_images[i],
        vkCreateImageView(device, &image_view_info, nullptr, &swapchain_image_views[i]);
    }


    acquire_semaphore = VK_NULL_HANDLE;
    submit_semaphore = VK_NULL_HANDLE;
    VkSemaphoreCreateInfo semaphore_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };
    VK_CHECK(vkCreateSemaphore(device, &semaphore_info, nullptr, &acquire_semaphore))
    VK_CHECK(vkCreateSemaphore(device, &semaphore_info, nullptr, &submit_semaphore))


    // Renderpass
    const VkAttachmentDescription attachments[] = {{
        .format = format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    }};

    VkAttachmentReference color_attachment_ref = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    const VkSubpassDescription subpass = {
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment_ref,
    };


    const VkRenderPassCreateInfo render_pass_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = attachments,
        .subpassCount = static_cast<uint32_t>(std::size(attachments)),
        .pSubpasses = &subpass,
    };
    VK_CHECK(vkCreateRenderPass(device, &render_pass_info, nullptr, &render_pass));


    // framebuffer
    VkFramebufferCreateInfo framebuffer_info = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = render_pass,
        .attachmentCount = 1,
        .width = 640,
        .height = 480,
        .layers = 1,
    };

    framebuffers.resize(swapchain_images.size());
    for (uint32_t i = 0; i < swapchain_images.size(); i++) {
        framebuffer_info.pAttachments = &swapchain_image_views[i];
        VK_CHECK(vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffers[i]));
    }

    // pipeline layout
    const VkPushConstantRange push_constants[]{{
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset = 0,
        .size = 4,
    }};
    VkPipelineLayoutCreateInfo pipeline_layout_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = push_constants,
    };
    VK_CHECK(vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &pipeline_layout))

    // pipeline
    VkPipelineCacheCreateInfo pipeline_cache_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
    };
    vkCreatePipelineCache(device, &pipeline_cache_info, nullptr, &pipeline_cache);

    VkShaderModule triangle_vertex_shader;
    VkShaderModule square_vertex_shader;
    VkShaderModule fragment_shader;

    std::vector<char> triangle_vert_shader =
        Platform::Filesystem::read_binary_file("Resources/triangle.vert.spv");
    std::vector<char> square_vert_shader =
        Platform::Filesystem::read_binary_file("Resources/square.vert.spv");
    std::vector<char> frag_shader =
        Platform::Filesystem::read_binary_file("Resources/color.frag.spv");
    VkShaderModuleCreateInfo triangle_vertex_shader_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = triangle_vert_shader.size(),
        .pCode = reinterpret_cast<const uint32_t*>(triangle_vert_shader.data()),
    };

    VkShaderModuleCreateInfo square_vertex_shader_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = square_vert_shader.size(),
        .pCode = reinterpret_cast<const uint32_t*>(square_vert_shader.data()),
    };

    VkShaderModuleCreateInfo fragment_shader_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = frag_shader.size(),
        .pCode = reinterpret_cast<const uint32_t*>(frag_shader.data()),
    };

    VK_CHECK(
        vkCreateShaderModule(device, &triangle_vertex_shader_info, nullptr, &triangle_vertex_shader)
    );
    VK_CHECK(
        vkCreateShaderModule(device, &square_vertex_shader_info, nullptr, &square_vertex_shader)
    );
    VK_CHECK(vkCreateShaderModule(device, &fragment_shader_info, nullptr, &fragment_shader));

    VkPipelineShaderStageCreateInfo pipeline_shader_stage_infos[]{
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = triangle_vertex_shader,
            .pName = "main",
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragment_shader,
            .pName = "main",
        },
    };

    VkPipelineColorBlendAttachmentState color_blend_attachment{
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo color_blend_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachment,
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    };

    VkPipelineRasterizationStateCreateInfo rasterization_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .lineWidth = 1.0f,
    };

    VkRect2D scissor{};
    VkViewport viewport{};

    VkPipelineViewportStateCreateInfo viewport_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };

    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(std::size(dynamic_states)),
        .pDynamicStates = dynamic_states,
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
    };

    VkPipelineMultisampleStateCreateInfo multisample_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    };

    VkGraphicsPipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = static_cast<uint32_t>(std::size(pipeline_shader_stage_infos)),
        .pStages = pipeline_shader_stage_infos,
        .pVertexInputState = &vertex_input_state_info,
        .pInputAssemblyState = &input_assembly_state_info,
        .pViewportState = &viewport_state_info,
        .pRasterizationState = &rasterization_state_info,
        .pMultisampleState = &multisample_state_info,
        .pColorBlendState = &color_blend_state_info,
        .pDynamicState = &dynamic_state_info,
        .layout = pipeline_layout,
        .renderPass = render_pass,
    };
    VK_CHECK(vkCreateGraphicsPipelines(
        device, pipeline_cache, 1, &pipeline_info, nullptr, &triangle_pipeline
    ))

    // Kinda messy, replace the triangle vertex shader with the square vertex shader and make another pipeline
    pipeline_shader_stage_infos[0] = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = square_vertex_shader,
        .pName = "main",
    };
    VK_CHECK(vkCreateGraphicsPipelines(
        device, pipeline_cache, 1, &pipeline_info, nullptr, &square_pipeline
    ))
}

Renderer::~Renderer() {
    // TODO
}

void Renderer::Render() {
    uint32_t image_index = 0;
    VK_CHECK(
        vkAcquireNextImageKHR(device, swapchain, 0, acquire_semaphore, VK_NULL_HANDLE, &image_index)
    );

    const VkCommandBufferAllocateInfo cbai = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    VK_CHECK(vkAllocateCommandBuffers(device, &cbai, &command_buffer));

    VkCommandBufferBeginInfo cbbi = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    VK_CHECK(vkBeginCommandBuffer(command_buffer, &cbbi))

    auto time = std::chrono::high_resolution_clock::now().time_since_epoch();
    auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time);
    float data = time_ms.count() / 1000.0f;
    vkCmdPushConstants(
        command_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float), &data
    );

    VkClearColorValue color = {0.3f, 0.77f, 0.5f, 1.0f};
    VkClearValue clear_value = {
        .color = color,
    };

    VkRenderPassBeginInfo render_pass_begin_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = render_pass,
        .framebuffer = framebuffers[image_index],
        // TODO: Get this from platform layer
        .renderArea = {.extent = {640, 480}},
        .clearValueCount = 1,
        .pClearValues = &clear_value,
    };

    vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    const VkRect2D scissor = {.extent = {640, 480}};
    const VkViewport viewport = {
        .width = 640,
        .height = 480,
        .maxDepth = 1.0f,
    };

    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, square_pipeline);
    vkCmdDraw(command_buffer, 4, 1, 0, 0);

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, triangle_pipeline);
    vkCmdDraw(command_buffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(command_buffer);

    VK_CHECK(vkEndCommandBuffer(command_buffer))

    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    const VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &acquire_semaphore,
        .pWaitDstStageMask = &wait_stage,
        .commandBufferCount = 1,
        .pCommandBuffers = &command_buffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &submit_semaphore,
    };
    VK_CHECK(vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE))


    VkPresentInfoKHR pi = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &submit_semaphore,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &image_index,
    };
    VK_CHECK(vkQueuePresentKHR(queue, &pi))

    VK_CHECK(vkDeviceWaitIdle(device));
    vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}
} // namespace Vee