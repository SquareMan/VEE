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
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <cassert>
#include <chrono>
#include <glm/common.hpp>
#include <imgui.h>
#include <numbers>
#include <set>
#include <vector>

namespace Vee {
Renderer::Renderer(const Platform::Window& window)
    : window(&window) {
    VULKAN_HPP_DEFAULT_DISPATCHER.init();

    {
#if _DEBUG
        constexpr bool enable_validation = true;
#else
        constexpr bool enable_validation = false;
#endif
        vkb::InstanceBuilder instance_builder;
        auto inst_res = instance_builder.set_app_name("VEE POC")
                            .set_app_version(0, 1, 0)
                            .enable_validation_layers(enable_validation)
                            .enable_extensions({
                                VK_KHR_SURFACE_EXTENSION_NAME,
                                VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
                            })
                            .set_debug_callback(&Vee::Vulkan::vk_debug_callback)
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
    vkb::PhysicalDevice vkb_gpu = selector.set_minimum_version(1, 3)
                                      .set_required_features_13(v13_features)
                                      .set_required_features_12(v12_features)
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
    vma::Allocator allocator = vma::createAllocator(allocator_info).value;

    // Command pool
    const vk::CommandPoolCreateInfo cpci(
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        vkb_device.get_queue_index(vkb::QueueType::graphics).value()
    );

    command_pool = vk::Device(device).createCommandPool(cpci).value;


    // swapchain
    std::vector<vk::SurfaceFormatKHR> surface_formats = gpu.getSurfaceFormatsKHR(surface).value;
    vk::Format format = {};
    for (const vk::SurfaceFormatKHR& surface_format : surface_formats) {
        if (surface_format.format == vk::Format::eB8G8R8A8Srgb) {
            format = surface_format.format;
            break;
        }
    }
    assert(format == vk::Format::eB8G8R8A8Srgb);

    auto [width, height] = window.get_size();
    swapchain.emplace(gpu, device, surface, format, width, height);

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
        .with_shader(fragment_shader)
        .with_shader(triangle_vertex_shader)
        .build(device);

    square_pipeline = Vulkan::PipelineBuilder()
        .with_cache(pipeline_cache)
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

    init_imgui();

    assert(swapchain != std::nullopt);
}

Renderer::~Renderer() {
    // TODO: Cleanup everything
    std::ignore = device.waitIdle();

#if _DEBUG
    static_cast<vk::Instance>(instance).destroyDebugUtilsMessengerEXT(debug_messenger_);
#endif

    for (CmdBuffer& command_buffer : command_buffers.buffer) {
        device.destroyFence(command_buffer.fence);
        device.destroySemaphore(command_buffer.acquire_semaphore);
        device.destroySemaphore(command_buffer.submit_semaphore);
    }
    device.destroyCommandPool(command_pool);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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
    record_commands(command_buffer.cmd, [&](vk::CommandBuffer cmd) {
        auto time = std::chrono::high_resolution_clock::now().time_since_epoch();
        auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time);
        float data = time_ms.count() / 1000.0f;

        // NOTE: triangle_pipeline and square_pipeline have the same layout, it shouldn't matter
        // which we use here.
        cmd.pushConstants(
            triangle_pipeline.layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(float), &data
        );

        // swapchain image transition
        transition_image(
            cmd,
            swapchain->images[image_index],
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal
        );

        vk::ClearValue clear_value({0.3f, 0.77f, 0.5f, 1.0f});
        vk::RenderingAttachmentInfo render_attachment = {
            swapchain->image_views[image_index],
            vk::ImageLayout::eColorAttachmentOptimal,
            {},
            {},
            {},
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            clear_value
        };
        vk::RenderingInfo render_info = {
            {}, {{}, {swapchain->width, swapchain->height}}, 1, 0, render_attachment, {}, {}
        };
        render(render_info, cmd, [&](vk::CommandBuffer cmd) {
            const vk::Rect2D scissor({{}, {swapchain->width, swapchain->height}});
            const vk::Viewport viewport(
                0,
                0,
                static_cast<float>(swapchain->width),
                static_cast<float>(swapchain->height),
                1.0f
            );

            cmd.setScissor(0, scissor);
            cmd.setViewport(0, viewport);

            cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, square_pipeline.pipeline);
            cmd.bindVertexBuffers(0, vertex_buffer.buffer, {{0}});
            cmd.bindIndexBuffer(index_buffer.buffer, 0, vk::IndexType::eUint16);
            cmd.drawIndexed(4, 1, 3, 0, 0);

            cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, triangle_pipeline.pipeline);
            cmd.bindVertexBuffers(0, vertex_buffer.buffer, {{0}});
            cmd.bindIndexBuffer(index_buffer.buffer, 0, vk::IndexType::eUint16);
            cmd.drawIndexed(3, 1, 0, 0, 0);

            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
        });

        // swapchain image transition
        transition_image(
            cmd,
            swapchain->images[image_index],
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ImageLayout::ePresentSrcKHR
        );
    });

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

void Renderer::init_imgui() {
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(window->glfw_window, true);

    // NOTE: (from imgui example)
    // The example only requires a single combined image sampler descriptor for the font image and
    // only uses one descriptor set (for that) If you wish to load e.g. additional textures you may
    // need to alter pools sizes.
    vk::DescriptorPoolSize pool_sizes[] = {
        {vk::DescriptorType::eCombinedImageSampler, 1},
    };

    vk::DescriptorPoolCreateInfo pool_info = {
        vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 1000, pool_sizes
    };
    vk::DescriptorPool pool = device.createDescriptorPool(pool_info).value;

    vk::PipelineRenderingCreateInfo pipeline_info = {{}, swapchain->format, {}, {}};
    ImGui_ImplVulkan_InitInfo init_info = {
        .Instance = instance,
        .PhysicalDevice = gpu,
        .Device = device,
        .Queue = graphics_queue,
        .DescriptorPool = pool,
        .MinImageCount = 3,
        .ImageCount = 3,
        .UseDynamicRendering = true,
        .PipelineRenderingCreateInfo = pipeline_info,
    };

    ImGui_ImplVulkan_LoadFunctions(
        [](const char* function_name, void* user_data) {
            return VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr(
                *static_cast<VkInstance*>(user_data), function_name
            );
        },
        &instance.instance
    );
    ImGui_ImplVulkan_Init(&init_info);
    ImGui_ImplVulkan_CreateFontsTexture();
}

void Renderer::record_commands(
    vk::CommandBuffer cmd, const std::function<void(vk::CommandBuffer cmd)>& func
) {
    std::ignore = cmd.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    func(cmd);
    std::ignore = cmd.end();
}
void Renderer::render(
    vk::RenderingInfo& rendering_info,
    vk::CommandBuffer cmd,
    const std::function<void(vk::CommandBuffer cmd)>& func
) {
    cmd.beginRendering(rendering_info);
    func(cmd);
    cmd.endRendering();
}

void Renderer::recreate_swapchain() {
    std::ignore = device.waitIdle();
    auto [width, height] = window->get_size();
    vk::Format old_format = swapchain->format;
    swapchain.emplace(gpu, device, surface, old_format, width, height);
}

void Renderer::transition_image(
    vk::CommandBuffer cmd, vk::Image image, vk::ImageLayout from, vk::ImageLayout to
) {
    // FIXME: This is the most inefficient transition
    vk::ImageMemoryBarrier2 image_barrier = {
        vk::PipelineStageFlagBits2::eAllCommands,
        vk::AccessFlagBits2::eMemoryWrite,
        vk::PipelineStageFlagBits2::eAllCommands,
        vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead,
        from,
        to,
        {},
        {},
        image,
        {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
    };

    vk::DependencyInfo dependency_info = {};
    dependency_info.setImageMemoryBarriers(image_barrier);
    cmd.pipelineBarrier2(dependency_info);
}
} // namespace Vee