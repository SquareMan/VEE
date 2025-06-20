//
// Created by Square on 10/27/2024.
//

#include "Renderer/RenderCtx.hpp"

#include "Assert.hpp"
#include "Renderer/VkUtil.hpp"
#include "Platform/Window.hpp"
#include "Vertex.hpp"

#include <functional>
#include <magic_enum/magic_enum.hpp>
#include <numbers>

namespace vee {
RenderCtx::RenderCtx(const platform::Window& window)
    : window(&window) {
    VULKAN_HPP_DEFAULT_DISPATCHER.init();

    {
#if _DEBUG
        constexpr bool enable_validation = true;
#else
        constexpr bool enable_validation = false;
#endif
        vkb::InstanceBuilder instance_builder;
        auto inst_res =
            instance_builder.set_app_name("HelloTriangle POC")
                .set_app_version(0, 1, 0)
                .enable_validation_layers(enable_validation)
                .enable_extensions({
                    VK_KHR_SURFACE_EXTENSION_NAME,
                    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
                })
                .set_debug_callback(&vee::vulkan::vk_debug_callback)
                .require_api_version(1, 3, 0)
                .build();

        instance = inst_res.value();
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vk::Instance(instance.instance));
    }

    const vk::Win32SurfaceCreateInfoKHR ci = {{}, GetModuleHandle(nullptr), window.get_handle()};
    surface = static_cast<vk::Instance>(instance).createWin32SurfaceKHR(ci).value;

    vk::PhysicalDeviceVulkan13Features v13_features;
    v13_features.synchronization2 = true;
    v13_features.dynamicRendering = true;

    vk::PhysicalDeviceVulkan12Features v12_features;
    v12_features.bufferDeviceAddress = true;
    v12_features.descriptorIndexing = true;

    vkb::PhysicalDeviceSelector selector(instance, surface);
    vkb::PhysicalDevice vkb_gpu =
        selector.set_minimum_version(1, 3)
            .set_required_features_13(v13_features)
            .set_required_features_12(v12_features)
            // NOTE: IMGUI's viewport mode is implemented to use the
            // vulkan dynamic rendering extension functions when dynamic
            // rendering is enabled rather than the vulkan 1.3 functions
            // so we need to enable the extension as well
            .add_required_extension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)
            .select()
            .value();
    gpu = vkb_gpu.physical_device;

    vkb::DeviceBuilder device_builder(vkb_gpu);
    vkb::Device vkb_device = device_builder.build().value();
    device = vkb_device.device;
    VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

    graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
    presentation_queue = vkb_device.get_queue(vkb::QueueType::present).value();

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
        instance.instance,
        VK_API_VERSION_1_3
    };
    allocator = vma::createAllocator(allocator_info).value;


    // Command pool
    const vk::CommandPoolCreateInfo cpci(
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        vkb_device.get_queue_index(vkb::QueueType::graphics).value()
    );

    command_pool = vk::Device(device).createCommandPool(cpci).value;


    // command buffers
    const vk::CommandBufferAllocateInfo command_buffer_info = {
        command_pool, vk::CommandBufferLevel::ePrimary, static_cast<uint32_t>(command_buffers.size())
    };

    std::vector<vk::CommandBuffer> tmp_command_buffers =
        device.allocateCommandBuffers(command_buffer_info).value;
    VASSERT(command_buffers.size() == tmp_command_buffers.size());
    for (uint32_t i = 0; i < tmp_command_buffers.size(); ++i) {
        command_buffers[i].fence = device.createFence({vk::FenceCreateFlagBits::eSignaled}).value;
        command_buffers[i].acquire_semaphore = device.createSemaphore({}).value;
        command_buffers[i].submit_semaphore = device.createSemaphore({}).value;
        command_buffers[i].cmd = tmp_command_buffers[i];
    }

    immediate_buffer_ =
        device.allocateCommandBuffers({command_pool, vk::CommandBufferLevel::ePrimary, 1}).value.front();
    immediate_fence_ = device.createFence({vk::FenceCreateFlagBits::eSignaled}).value;


    // swapchain
    std::vector<vk::SurfaceFormatKHR> surface_formats = gpu.getSurfaceFormatsKHR(surface).value;
    vk::Format format = {};
    for (const vk::SurfaceFormatKHR& surface_format : surface_formats) {
        if (surface_format.format == vk::Format::eB8G8R8A8Srgb) {
            format = surface_format.format;
            break;
        }
    }
    VASSERT(format == vk::Format::eB8G8R8A8Srgb, "Expected surface format unavailable. Found vk::Format::{}", magic_enum::enum_name<vk::Format>(format));

    auto [width, height] = window.get_size();
    new (&swapchain) Swapchain(gpu, device, surface, format, width, height);
    log_info("Created swapchain with size {}x{}", width, height);

    pipeline_cache = device.createPipelineCache({}).value;

    vk::DescriptorPoolSize pool_sizes[] = {{vk::DescriptorType::eCombinedImageSampler, 1000}};
    descriptor_pool =
        device
            .createDescriptorPool({vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 1000, pool_sizes})
            .value;

    {
        // 1MB staging buffer
        vk::BufferCreateInfo buffer_info({}, 1024 * 1024, vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive);
        auto [buf, alloc] =
            allocator
                .createBuffer(buffer_info, {vma::AllocationCreateFlagBits::eHostAccessSequentialWrite, vma::MemoryUsage::eAuto})
                .value;
        new (&staging_buffer) Buffer(buf, alloc, allocator);
    }

    float a = 4.0f * std::numbers::pi_v<float> / 3.0f;
    float b = 2.0f * std::numbers::pi_v<float> / 3.0f;
    float c = 0;
    const Vertex vertices[] = {
        {{std::cos(a), std::sin(a)}, {1.0f, 0.0f, 0.0f}, {std::cos(a), std::sin(a)}},
        {{std::cos(c), std::sin(c)}, {0, 1.0f, 0.0f}, {std::cos(c), std::sin(c)}},
        {{std::cos(b), std::sin(b)}, {0.0f, 0.0f, 1.0f}, {std::cos(b), std::sin(b)}},
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    };
    const uint16_t indices[] = {0, 1, 2, 3, 4, 5, 6};
    {
        std::ignore = allocator.copyMemoryToAllocation(vertices, staging_buffer.allocation, 0, sizeof(vertices));
        std::ignore = allocator.copyMemoryToAllocation(indices, staging_buffer.allocation, sizeof(vertices), sizeof(indices));
    }

    {
        vk::BufferCreateInfo buffer_info = {
            {}, sizeof(vertices) + sizeof(indices), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::SharingMode::eExclusive
        };
        {
            auto [buf, alloc] =
                allocator
                    .createBuffer(buffer_info, {vma::AllocationCreateFlagBits::eDedicatedMemory, vma::MemoryUsage::eAuto})
                    .value;
            new (&vertex_buffer) Buffer(buf, alloc, allocator);
        }

        buffer_info.setUsage(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer);
        {
            auto [buf, alloc] =
                allocator
                    .createBuffer(buffer_info, {vma::AllocationCreateFlagBits::eDedicatedMemory, vma::MemoryUsage::eGpuOnly})
                    .value;
            new (&index_buffer) Buffer(buf, alloc, allocator);
        }
    }
    // Copy from staging to final buffers
    immediate_submit([&](vk::CommandBuffer cmd) {
        cmd.copyBuffer(staging_buffer.buffer, vertex_buffer.buffer, {{0, 0, sizeof(vertices)}});
        cmd.copyBuffer(staging_buffer.buffer, index_buffer.buffer, {{sizeof(vertices), 0, sizeof(indices)}});
    });
}

void RenderCtx::recreate_swapchain() {
    std::ignore = device.waitIdle();
    auto [width, height] = window->get_size();
    vk::Format old_format = swapchain.format;

    swapchain.~Swapchain();
    new (&swapchain) Swapchain(gpu, device, surface, old_format, width, height);
}

void RenderCtx::immediate_submit(const std::function<void(vk::CommandBuffer cmd)>& func) const {
    std::ignore = device.waitForFences(immediate_fence_, true, UINT64_MAX);
    std::ignore = device.resetFences(immediate_fence_);

    std::ignore = immediate_buffer_.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    func(immediate_buffer_);
    std::ignore = immediate_buffer_.end();

    vk::SubmitInfo submit_info = {{}, {}, immediate_buffer_};
    std::ignore = graphics_queue.submit(submit_info, immediate_fence_);
}
} // namespace vee