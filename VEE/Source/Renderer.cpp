//
// Created by Square on 9/28/2024.
//

#include "Renderer.hpp"

#include "VeeCore.hpp"

#define VOLK_IMPLEMENTATION
#include <Volk/volk.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_win32.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "Platform/Filesystem.hpp"
#include "Renderer/Shader.hpp"

#include <Renderer/VkUtil.hpp>
#include <set>

namespace Vee {
Renderer::Renderer(const Platform::Window& window)
    : window(&window) {
#if !defined(VK_KHR_win32_surface) || !defined(VK_KHR_surface)
#error Renderer requires win32 platform and vulkan surface extensions
#endif

    VK_CHECK(volkInitialize());

    // clang-format off
    instance = Vulkan::InstanceBuilder()
        .with_application_name("VEE POC")
        .with_application_version(VK_MAKE_API_VERSION(0, 0, 1, 0))
        .with_layers({
#if _DEBUG
            "VK_LAYER_KHRONOS_validation",
#endif
        })
        .with_extensions({
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
            VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(VK_EXT_debug_utils)
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
        })
        .build();
    // clang-format on

#if _DEBUG
    if (!instance->is_extension_enabled(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        // TODO log error
        VEE_DEBUGBREAK();
    } else {
        // TODO: vkCreateDebugUtilsMessengerEXT
    }
#endif


    VkSurfaceKHR surface = VK_NULL_HANDLE;
    const VkWin32SurfaceCreateInfoKHR ci = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hinstance = GetModuleHandle(nullptr),
        .hwnd = window.get_handle(),
    };
    VK_CHECK(vkCreateWin32SurfaceKHR(instance->vk_instance, &ci, nullptr, &surface))

    uint32_t num_physical_devices = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(instance->vk_instance, &num_physical_devices, nullptr))
    std::vector<VkPhysicalDevice> physical_devices(num_physical_devices);
    VK_CHECK(vkEnumeratePhysicalDevices(
        instance->vk_instance, &num_physical_devices, physical_devices.data()
    ))

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

    std::optional<uint32_t> graphics_family_index;
    std::optional<uint32_t> presentation_family_index;

    for (uint32_t idx = 0; idx < queue_families.size(); ++idx) {
        if (!graphics_family_index.has_value()
            && queue_families[idx].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphics_family_index = idx;
        }

        if (!presentation_family_index.has_value()) {
            VkBool32 supports_present = VK_FALSE;
            VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, idx, surface, &supports_present))
            if (supports_present == VK_TRUE) {
                presentation_family_index = idx;
            }
        }
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
        Vulkan::filter_extensions(available_property_names, requested_property_names);

    assert(graphics_family_index.has_value());
    assert(presentation_family_index.has_value());

    std::set<uint32_t> queue_family_indices({*graphics_family_index, *presentation_family_index});
    const float priority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> qcis;
    qcis.reserve(queue_family_indices.size());
    for (uint32_t unique_index : queue_family_indices) {
        qcis.push_back({
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = unique_index,
            .queueCount = 1,
            .pQueuePriorities = &priority,
        });
    }

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

    vkGetDeviceQueue(device, *graphics_family_index, 0, &graphics_queue);
    vkGetDeviceQueue(device, *presentation_family_index, 0, &presentation_queue);


    const VkCommandPoolCreateInfo cpci = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = *graphics_family_index,
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
    assert(format == VK_FORMAT_B8G8R8A8_SRGB);

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
    auto [width, height] = window.get_size();
    VkFramebufferCreateInfo framebuffer_info = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = render_pass,
        .attachmentCount = 1,
        .width = width,
        .height = height,
        .layers = 1,
    };

    framebuffers.resize(swapchain_images.size());
    for (uint32_t i = 0; i < swapchain_images.size(); i++) {
        framebuffer_info.pAttachments = &swapchain_image_views[i];
        VK_CHECK(vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffers[i]));
    }

    // pipelines
    VkPipelineCacheCreateInfo pipeline_cache_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
    };
    VkPipelineCache pipeline_cache;
    vkCreatePipelineCache(device, &pipeline_cache_info, nullptr, &pipeline_cache);

    std::vector<char> triangle_vertex_shader_code =
        Platform::Filesystem::read_binary_file("Resources/triangle.vert.spv");
    std::vector<char> square_vertex_shader_code =
        Platform::Filesystem::read_binary_file("Resources/square.vert.spv");
    std::vector<char> frag_shader_code =
        Platform::Filesystem::read_binary_file("Resources/color.frag.spv");

    Vulkan::Shader fragment_shader = {device, VK_SHADER_STAGE_FRAGMENT_BIT, frag_shader_code};
    Vulkan::Shader triangle_vertex_shader = {
        device, VK_SHADER_STAGE_VERTEX_BIT, triangle_vertex_shader_code
    };
    Vulkan::Shader square_vertex_shader = {
        device, VK_SHADER_STAGE_VERTEX_BIT, square_vertex_shader_code
    };

    // clang-format off
    triangle_pipeline = Vulkan::PipelineBuilder()
        .with_cache(pipeline_cache)
        .with_renderpass(render_pass)
        .with_shader(fragment_shader)
        .with_shader(triangle_vertex_shader)
        .build(device);

    square_pipeline = Vulkan::PipelineBuilder()
        .with_cache(pipeline_cache)
        .with_renderpass(render_pass)
        .with_shader(fragment_shader)
        .with_shader(square_vertex_shader)
        .build(device);
    // clang-format on


    // command buffers
    VkFenceCreateInfo fence_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    VkSemaphoreCreateInfo semaphore_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };
    const VkCommandBufferAllocateInfo command_buffer_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    for (CmdBuffer& command_buffer : command_buffers.buffer) {
        VK_CHECK(vkCreateFence(device, &fence_info, nullptr, &command_buffer.fence));
        VK_CHECK(
            vkCreateSemaphore(device, &semaphore_info, nullptr, &command_buffer.acquire_semaphore)
        )
        VK_CHECK(
            vkCreateSemaphore(device, &semaphore_info, nullptr, &command_buffer.submit_semaphore)
        )
        VK_CHECK(vkAllocateCommandBuffers(device, &command_buffer_info, &command_buffer.cmd));
    }
}

Renderer::~Renderer() {
    // TODO: Cleanup everything
    VK_CHECK(vkDeviceWaitIdle(device))
    for (CmdBuffer& command_buffer : command_buffers.buffer) {
        vkFreeCommandBuffers(device, command_pool, 1, &command_buffer.cmd);
        vkDestroyFence(device, command_buffer.fence, nullptr);
    }
}

void Renderer::Render() {
    CmdBuffer& command_buffer = command_buffers.get_next();
    uint32_t image_index = 0;

    VK_CHECK(vkWaitForFences(device, 1, &command_buffer.fence, VK_TRUE, UINT64_MAX))
    VK_CHECK(vkResetFences(device, 1, &command_buffer.fence))
    VK_CHECK(vkAcquireNextImageKHR(
        device, swapchain, 0, command_buffer.acquire_semaphore, VK_NULL_HANDLE, &image_index
    ));

    VkCommandBufferBeginInfo cbbi = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    VK_CHECK(vkResetCommandBuffer(command_buffer.cmd, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT)
    )
    VK_CHECK(vkBeginCommandBuffer(command_buffer.cmd, &cbbi))

    auto time = std::chrono::high_resolution_clock::now().time_since_epoch();
    auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time);
    float data = time_ms.count() / 1000.0f;

    // NOTE: triangle_pipeline and square_pipeline have the same layout, it shouldn't matter which
    // we use here.
    vkCmdPushConstants(
        command_buffer.cmd,
        triangle_pipeline.layout,
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        sizeof(float),
        &data
    );

    VkClearColorValue color = {0.3f, 0.77f, 0.5f, 1.0f};
    VkClearValue clear_value = {
        .color = color,
    };

    auto [width, height] = window->get_size();
    VkRenderPassBeginInfo render_pass_begin_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = render_pass,
        .framebuffer = framebuffers[image_index],
        // TODO: Get this from platform layer
        .renderArea = {.extent = {width, height}},
        .clearValueCount = 1,
        .pClearValues = &clear_value,
    };

    vkCmdBeginRenderPass(command_buffer.cmd, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    const VkRect2D scissor = {.extent = {width, height}};
    const VkViewport viewport = {
        .width = static_cast<float>(width),
        .height = static_cast<float>(height),
        .maxDepth = 1.0f,
    };

    vkCmdSetScissor(command_buffer.cmd, 0, 1, &scissor);
    vkCmdSetViewport(command_buffer.cmd, 0, 1, &viewport);

    vkCmdBindPipeline(command_buffer.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, square_pipeline.pipeline);
    vkCmdDraw(command_buffer.cmd, 4, 1, 0, 0);

    vkCmdBindPipeline(
        command_buffer.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, triangle_pipeline.pipeline
    );
    vkCmdDraw(command_buffer.cmd, 3, 1, 0, 0);

    vkCmdEndRenderPass(command_buffer.cmd);

    VK_CHECK(vkEndCommandBuffer(command_buffer.cmd))

    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    const VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &command_buffer.acquire_semaphore,
        .pWaitDstStageMask = &wait_stage,
        .commandBufferCount = 1,
        .pCommandBuffers = &command_buffer.cmd,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &command_buffer.submit_semaphore,
    };
    VK_CHECK(vkQueueSubmit(graphics_queue, 1, &submit_info, command_buffer.fence))


    VkPresentInfoKHR pi = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &command_buffer.submit_semaphore,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &image_index,
    };
    VK_CHECK(vkQueuePresentKHR(presentation_queue, &pi))
}
} // namespace Vee