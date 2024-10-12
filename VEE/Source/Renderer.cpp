//
// Created by Square on 9/28/2024.
//

#include "Renderer.hpp"

#include "Platform/Filesystem.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/VkUtil.hpp"
#include "VeeCore.hpp"
#include "Vertex.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <glm/common.hpp>
#include <numbers>
#include <set>
#include <vector>

namespace Vee {
Renderer::Renderer(const Platform::Window& window)
    : window(&window) {
#if !defined(VK_KHR_win32_surface) || !defined(VK_KHR_surface)
#error Renderer requires win32 platform and vulkan surface extensions
#endif

    VULKAN_HPP_DEFAULT_DISPATCHER.init();
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
    if (instance->is_extension_enabled(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        vk::DebugUtilsMessengerCreateInfoEXT debug_messenger_info = {
            {},
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
                | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
                | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
                | vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral,
            &Vee::Vulkan::vk_debug_callback
        };
        debug_messenger_ =
            instance->vk_instance.createDebugUtilsMessengerEXT(debug_messenger_info).value;
    } else {
        // TODO log error
        VEE_DEBUGBREAK();
    }
#endif


    const vk::Win32SurfaceCreateInfoKHR ci = {{}, GetModuleHandle(nullptr), window.get_handle()};
    surface = instance->vk_instance.createWin32SurfaceKHR(ci).value;
    std::vector<vk::PhysicalDevice> physical_devices =
        instance->vk_instance.enumeratePhysicalDevices().value;

    assert(!physical_devices.empty());
    gpu = physical_devices[0];

    vk::PhysicalDeviceProperties physical_properties = gpu.getProperties();
    VkPhysicalDeviceFeatures physical_features = gpu.getFeatures();

    std::vector<vk::QueueFamilyProperties> queue_families = gpu.getQueueFamilyProperties();

    std::optional<uint32_t> graphics_family_index;
    std::optional<uint32_t> presentation_family_index;

    for (uint32_t idx = 0; idx < queue_families.size(); ++idx) {
        if (!graphics_family_index.has_value()
            && queue_families[idx].queueFlags & vk::QueueFlagBits::eGraphics) {
            graphics_family_index = idx;
        }

        if (!presentation_family_index.has_value()
            && gpu.getSurfaceSupportKHR(idx, surface).value) {
            presentation_family_index = idx;
        }
    }

    std::vector<vk::ExtensionProperties> properties =
        gpu.enumerateDeviceExtensionProperties().value;

    std::vector<const char*> available_extension_names;
    std::ranges::transform(
        properties,
        std::back_inserter(available_extension_names),
        [](const auto& property) { return property.extensionName; }
    );

    std::vector<const char*> requested_extension_names = {"VK_KHR_swapchain"};
    std::vector<const char*> enabled_extension_names =
        Vulkan::filter_extensions(available_extension_names, requested_extension_names);

    assert(graphics_family_index.has_value());
    assert(presentation_family_index.has_value());

    std::set<uint32_t> queue_family_indices({*graphics_family_index, *presentation_family_index});
    const float priority = 1.0f;
    std::vector<vk::DeviceQueueCreateInfo> qcis;
    qcis.reserve(queue_family_indices.size());
    for (uint32_t unique_index : queue_family_indices) {
        qcis.push_back({{}, unique_index, 1, &priority});
    }

    const vk::DeviceCreateInfo dci({}, qcis, {}, enabled_extension_names);

    device = gpu.createDevice(dci).value;
    VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

    vma::VulkanFunctions vulkan_functions;
    vulkan_functions.vkGetInstanceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr;
    vulkan_functions.vkGetDeviceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceProcAddr;
    vma::AllocatorCreateInfo allocator_info = {
        vma::AllocatorCreateFlagBits::eExtMemoryBudget,
        gpu,
        device,
        {},
        nullptr,
        nullptr,
        {},
        &vulkan_functions,
        instance->vk_instance,
        VK_API_VERSION_1_3
    };
    vma::Allocator allocator = vma::createAllocator(allocator_info).value;


    graphics_queue = device.getQueue(*graphics_family_index, 0);
    presentation_queue = device.getQueue(*presentation_family_index, 0);


    // Command pool
    const vk::CommandPoolCreateInfo cpci(
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer, *graphics_family_index
    );

    command_pool = device.createCommandPool(cpci).value;


    // Renderpass
    std::vector<vk::SurfaceFormatKHR> surface_formats = gpu.getSurfaceFormatsKHR(surface).value;
    vk::Format format = {};
    for (const vk::SurfaceFormatKHR& surface_format : surface_formats) {
        if (surface_format.format == vk::Format::eB8G8R8A8Srgb) {
            format = surface_format.format;
            break;
        }
    }
    assert(format == vk::Format::eB8G8R8A8Srgb);


    const vk::AttachmentDescription attachments[] = {{
        {},
        format,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        {},
        {},
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::ePresentSrcKHR,
    }};

    vk::AttachmentReference color_attachment_ref(0, vk::ImageLayout::eColorAttachmentOptimal);

    const vk::SubpassDescription subpass_description({}, {}, {}, color_attachment_ref);
    const vk::RenderPassCreateInfo render_pass_info({}, attachments, subpass_description);
    render_pass = device.createRenderPass(render_pass_info).value;
    auto [width, height] = window.get_size();
    swapchain.emplace(gpu, device, surface, format, render_pass, width, height);

    // pipelines
    vk::PipelineCache pipeline_cache = device.createPipelineCache({}).value;

    std::vector<char> triangle_vertex_shader_code =
        Platform::Filesystem::read_binary_file("Resources/triangle.vert.spv");
    std::vector<char> square_vertex_shader_code =
        Platform::Filesystem::read_binary_file("Resources/square.vert.spv");
    std::vector<char> frag_shader_code =
        Platform::Filesystem::read_binary_file("Resources/color.frag.spv");

    Vulkan::Shader fragment_shader = {device, vk::ShaderStageFlagBits::eFragment, frag_shader_code};
    Vulkan::Shader triangle_vertex_shader = {
        device, vk::ShaderStageFlagBits::eVertex, triangle_vertex_shader_code
    };
    Vulkan::Shader square_vertex_shader = {
        device, vk::ShaderStageFlagBits::eVertex, square_vertex_shader_code
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
    const vk::CommandBufferAllocateInfo command_buffer_info = {
        command_pool,
        vk::CommandBufferLevel::ePrimary,
        static_cast<uint32_t>(command_buffers.size())
    };

    std::vector<vk::CommandBuffer> tmp_command_buffers =
        device.allocateCommandBuffers(command_buffer_info).value;
    assert(command_buffers.size() == tmp_command_buffers.size());
    for (uint32_t i = 0; i < tmp_command_buffers.size(); ++i) {
        command_buffers[i].fence = device.createFence({vk::FenceCreateFlagBits::eSignaled}).value;
        command_buffers[i].acquire_semaphore = device.createSemaphore({}).value;
        command_buffers[i].submit_semaphore = device.createSemaphore({}).value;
        command_buffers[i].cmd = tmp_command_buffers[i];
    }


    // vertex and staging buffers
    const Vertex vertices[] = {
        {{4.0f * std::numbers::pi_v<float> / 3.0f, 4.0f * std::numbers::pi_v<float> / 3.0f},
         {1.0f, 0.0f, 0.0f}},
        {{2.0f * std::numbers::pi_v<float> / 3.0f, 2.0f * std::numbers::pi_v<float> / 3.0f},
         {0.0f, 1.0f, 1.0f}},
        {{0, 0}, {0.0f, 0.0f, 1.0f}},
        {{-0.25f, -0.25f}, {1.0f, 0.0f, 0.0f}},
        {{0.25f, -0.25f}, {0.0f, 1.0f, 0.0f}},
        {{0.25f, 0.25f}, {1.0f, 0.0f, 0.0f}},
        {{-0.25f, 0.25f}, {0.0f, 1.0f, 0.0f}},

    };
    const uint16_t indices[] = {0, 1, 2, 3, 4, 5, 6};
    {
        vk::BufferCreateInfo buffer_info(
            {},
            sizeof(vertices) + sizeof(indices),
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::SharingMode::eExclusive
        );
        auto [buf, alloc] = allocator
                                .createBuffer(
                                    buffer_info,
                                    {vma::AllocationCreateFlagBits::eHostAccessSequentialWrite,
                                     vma::MemoryUsage::eAuto}
                                )
                                .value;
        new (&staging_buffer) Buffer(buf, alloc, allocator);

        std::ignore = allocator.copyMemoryToAllocation(
            vertices, staging_buffer.allocation, 0, sizeof(vertices)
        );
        std::ignore = allocator.copyMemoryToAllocation(
            indices, staging_buffer.allocation, sizeof(vertices), sizeof(indices)
        );
    }

    {
        vk::BufferCreateInfo buffer_info = {
            {},
            sizeof(vertices) + sizeof(indices),
            vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
            vk::SharingMode::eExclusive
        };
        {
            auto [buf, alloc] =
                allocator
                    .createBuffer(
                        buffer_info,
                        {vma::AllocationCreateFlagBits::eDedicatedMemory, vma::MemoryUsage::eAuto}
                    )
                    .value;
            new (&vertex_buffer) Buffer(buf, alloc, allocator);
        }

        buffer_info.setUsage(
            vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer
        );
        {
            auto [buf, alloc] = allocator
                                    .createBuffer(
                                        buffer_info,
                                        {vma::AllocationCreateFlagBits::eDedicatedMemory,
                                         vma::MemoryUsage::eGpuOnly}
                                    )
                                    .value;
            new (&index_buffer) Buffer(buf, alloc, allocator);
        }
    }

    // Copy from staging to final buffers
    {
        CmdBuffer& cmd_buffer = command_buffers.buffer[0];

        std::ignore = device.resetFences(cmd_buffer.fence);
        std::ignore = cmd_buffer.cmd.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
        cmd_buffer.cmd.copyBuffer(
            staging_buffer.buffer, vertex_buffer.buffer, {{0, 0, sizeof(vertices)}}
        );
        cmd_buffer.cmd.copyBuffer(
            staging_buffer.buffer, index_buffer.buffer, {{sizeof(vertices), 0, sizeof(indices)}}
        );
        std::ignore = cmd_buffer.cmd.end();

        vk::SubmitInfo submit_info({}, {}, cmd_buffer.cmd);
        std::ignore = graphics_queue.submit(submit_info, cmd_buffer.fence);
    }

    auto info = allocator.getHeapBudgets();

    assert(swapchain != std::nullopt);
}

Renderer::~Renderer() {
    // TODO: Cleanup everything
    std::ignore = device.waitIdle();

#if _DEBUG
    instance->vk_instance.destroyDebugUtilsMessengerEXT(debug_messenger_);
#endif

    for (CmdBuffer& command_buffer : command_buffers.buffer) {
        device.destroyFence(command_buffer.fence);
        device.destroySemaphore(command_buffer.acquire_semaphore);
        device.destroySemaphore(command_buffer.submit_semaphore);
    }
    device.destroyCommandPool(command_pool);
}

void Renderer::Render() {
    CmdBuffer& command_buffer = command_buffers.get_next();

    std::ignore = device.waitForFences(command_buffer.fence, true, UINT64_MAX);
    uint32_t image_index = UINT32_MAX;
    {
        // Opt out of return value transformation to avoid asserting on
        // vk::Result::eErrorOutOfDateKHR
        const vk::Result result = device.acquireNextImageKHR(
            swapchain->handle, 0, command_buffer.acquire_semaphore, nullptr, &image_index
        );
        switch (result) {
        case vk::Result::eSuccess:
            break;
        case vk::Result::eTimeout:
            assert(false);
            return;
        case vk::Result::eNotReady:
            assert(false);
            return;
        case vk::Result::eSuboptimalKHR:
        case vk::Result::eErrorOutOfDateKHR:
            recreate_swapchain();
        default:
            assert(false);
            return;
        }
    }
    assert(image_index != UINT32_MAX);
    std::ignore = device.resetFences(command_buffer.fence);

    std::ignore = command_buffer.cmd.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    std::ignore = command_buffer.cmd.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    auto time = std::chrono::high_resolution_clock::now().time_since_epoch();
    auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time);
    float data = time_ms.count() / 1000.0f;

    // NOTE: triangle_pipeline and square_pipeline have the same layout, it shouldn't matter which
    // we use here.
    command_buffer.cmd.pushConstants(
        triangle_pipeline.layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(float), &data
    );

    vk::ClearValue clear_value({0.3f, 0.77f, 0.5f, 1.0f});

    const vk::RenderPassBeginInfo render_pass_begin_info(
        render_pass,
        swapchain->framebuffers[image_index],
        {{}, {swapchain->width, swapchain->height}},
        clear_value
    );

    command_buffer.cmd.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
    {
        const vk::Rect2D scissor({{}, {swapchain->width, swapchain->height}});
        const vk::Viewport viewport(
            0, 0, static_cast<float>(swapchain->width), static_cast<float>(swapchain->height), 1.0f
        );

        command_buffer.cmd.setScissor(0, scissor);
        command_buffer.cmd.setViewport(0, viewport);

        command_buffer.cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, square_pipeline.pipeline);
        command_buffer.cmd.bindVertexBuffers(0, vertex_buffer.buffer, {{0}});
        command_buffer.cmd.bindIndexBuffer(index_buffer.buffer, 0, vk::IndexType::eUint16);
        command_buffer.cmd.drawIndexed(4, 1, 3, 0, 0);

        command_buffer.cmd.bindPipeline(
            vk::PipelineBindPoint::eGraphics, triangle_pipeline.pipeline
        );
        command_buffer.cmd.bindVertexBuffers(0, vertex_buffer.buffer, {{0}});
        command_buffer.cmd.bindIndexBuffer(index_buffer.buffer, 0, vk::IndexType::eUint16);
        command_buffer.cmd.drawIndexed(3, 1, 0, 0, 0);
    }
    command_buffer.cmd.endRenderPass();
    std::ignore = command_buffer.cmd.end();

    vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    const vk::SubmitInfo submit_info(
        command_buffer.acquire_semaphore,
        wait_stage,
        command_buffer.cmd,
        command_buffer.submit_semaphore
    );
    std::ignore = graphics_queue.submit(submit_info, command_buffer.fence);


    const vk::PresentInfoKHR pi(command_buffer.submit_semaphore, swapchain->handle, image_index);
    {
        // Opt out of return value transformation to avoid asserting on
        // vk::Result::eErrorOutOfDateKHR
        switch (presentation_queue.presentKHR(&pi)) {
        case vk::Result::eSuccess:
            break;
        case vk::Result::eSuboptimalKHR:
        case vk::Result::eErrorOutOfDateKHR:
            recreate_swapchain();
            break;
        default:
            assert(false);
        }
    }
}

void Renderer::recreate_swapchain() {
    std::ignore = device.waitIdle();
    auto [width, height] = window->get_size();
    vk::Format old_format = swapchain->format;
    swapchain.emplace(gpu, device, surface, old_format, render_pass, width, height);
}
} // namespace Vee