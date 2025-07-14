cmake_minimum_required(VERSION 3.28.0)

add_library(VeeRuntime)
enable_language(ASM)
# I don't know why this isn't brought in from linking to the library
target_include_directories(VeeRuntime PUBLIC ${VulkanMemoryAllocator-Hpp_SOURCE_DIR}/VulkanMemoryAllocator/include)
target_link_libraries(VeeRuntime
        PUBLIC
        VeeCore
        VulkanMemoryAllocator-Hpp
        glm::glm
        vk-bootstrap::vk-bootstrap
        glfw
        EnTT::EnTT
        $<$<CONFIG:Debug>:VeeEditor;imgui>

        PRIVATE
        magic_enum
        stb
        Tracy::TracyClient
)
target_include_directories(VeeRuntime PRIVATE Private/)

target_sources(VeeRuntime
        PUBLIC
        FILE_SET headers TYPE HEADERS
        BASE_DIRS Public/
        FILES
        Public/Application.hpp
        Public/Fibers.hpp
        Public/IApplication.hpp
        Public/Keys.hpp
        Public/MakeSharedEnabler.hpp
        Public/Renderer.hpp
        Public/RingBuffer.hpp
        Public/Transform.h
        Public/Vertex.hpp
        Public/Components/CameraComponent.hpp
        Public/Components/SpriteRendererComponent.hpp
        Public/Engine/Engine.hpp
        Public/Engine/Entity.h
        Public/Engine/Material.hpp
        Public/Engine/FrameImagePass.hpp
        Public/RenderGraph/DirectSource.hpp
        Public/RenderGraph/Handles.hpp
        Public/RenderGraph/ImageResource.hpp
        Public/RenderGraph/Pass.hpp
        Public/RenderGraph/RenderGraph.hpp
        Public/RenderGraph/RenderGraphBuilder.hpp
        Public/RenderGraph/Sink.hpp
        Public/RenderGraph/Source.hpp
        Public/Engine/SceneRenderPass.hpp
        Public/Engine/Texture.hpp
        Public/Engine/Sprite.hpp
        Public/Engine/World.h
        Public/Platform/Filesystem.hpp
        Public/Platform/Window.hpp
        Public/Platform/WindowHandle.hpp
        Public/Renderer/Buffer.hpp
        Public/Renderer/Image.hpp
        Public/Renderer/Pipeline.hpp
        Public/Renderer/RenderCtx.hpp
        Public/Renderer/Shader.hpp
        Public/Renderer/Swapchain.hpp
        Public/Renderer/VkUtil.hpp

        PRIVATE
        FILE_SET private_headers TYPE HEADERS
        BASE_DIRS Private/
        FILES

        PRIVATE
        Private/Application.cpp
        Private/Fibers${VEE_PLATFORM_SUFFIX}_x64.s
        Private/Fibers${VEE_PLATFORM_SUFFIX}.cpp
        Private/Renderer.cpp
        Private/RingBuffer.cpp
        Private/stb_image_impl.cpp
        Private/Transform.cpp
        Private/Components/CameraComponent.cpp
        Private/Components/SpriteRendererComponent.cpp
        Private/Engine/Engine.cpp
        Private/Engine/Entity.cpp
        Private/Engine/Material.cpp
        Private/Engine/FrameImagePass.cpp
        Private/RenderGraph/Pass.cpp
        Private/RenderGraph/RenderGraph.cpp
        Private/RenderGraph/RenderGraphBuilder.cpp
        Private/RenderGraph/Sink.cpp
        Private/RenderGraph/Source.cpp
        Private/Engine/SceneRenderPass.cpp
        Private/Engine/Texture.cpp
        Private/Engine/Sprite.cpp
        Private/Engine/World.cpp
        Private/Platform/Filesystem.cpp
        Private/Platform/Window.cpp
        Private/Renderer/Buffer.cpp
        Private/Renderer/Image.cpp
        Private/Renderer/Pipeline.cpp
        Private/Renderer/RenderCtx.cpp
        Private/Renderer/Swapchain.cpp
        Private/Renderer/Shader.cpp
        Private/Renderer/VkUtil.cpp
)
set_property(SOURCE Private/Fibers_win32_x64.s APPEND PROPERTY COMPILE_OPTIONS "-x" "assembler-with-cpp" "-DASSEMBLY")

target_compile_definitions(VeeRuntime PUBLIC
        VULKAN_HPP_NO_EXCEPTIONS
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VMA_STATIC_VULKAN_FUNCTIONS=0
        VMA_DYNAMIC_VULKAN_FUNCTIONS=1
        $<$<CONFIG:Debug>:VEE_WITH_EDITOR>
)
if (WIN32)
    target_compile_definitions(VeeRuntime PUBLIC VK_USE_PLATFORM_WIN32_KHR)
elseif (LINUX)
    target_compile_definitions(VeeRuntime PUBLIC VK_USE_PLATFORM_XLIB_KHR)
endif ()
