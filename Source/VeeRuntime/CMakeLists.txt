cmake_minimum_required(VERSION 3.28.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(VeeRuntime)
# I don't know why this isn't brought in from linking to the library
target_include_directories(VeeRuntime PUBLIC ${VulkanMemoryAllocator-Hpp_SOURCE_DIR}/VulkanMemoryAllocator/include)
target_link_libraries(VeeRuntime
        PUBLIC
        VulkanMemoryAllocator-Hpp
        glm::glm
        vk-bootstrap::vk-bootstrap
        glfw
        imgui
        EnTT::EnTT

        PRIVATE
        Boost::log
        Boost::log_setup
        magic_enum
)
target_include_directories(VeeRuntime PRIVATE Private/)

target_sources(VeeRuntime
        PUBLIC
        FILE_SET modules TYPE CXX_MODULES FILES
        Public/Logging.ixx

        FILE_SET headers TYPE HEADERS
        BASE_DIRS Public/
        FILES
        Public/Renderer.hpp
        Public/RingBuffer.hpp
        Public/Vertex.hpp
        Public/Renderer/VkUtil.hpp
        Public/Renderer/Pipeline.hpp
        Public/Renderer/Shader.hpp
        Public/Renderer/Swapchain.hpp
        Public/Renderer/Buffer.hpp
        Public/Renderer/Image.hpp
        Public/VeeCore.hpp
        Public/Platform/Filesystem.hpp
        Public/Platform/Window.hpp
        Public/Platform/WindowHandle.hpp
        Public/Renderer/IRenderer.hpp
        Public/Renderer/RenderCtx.hpp
        Public/Engine/Engine.hpp
        Public/Engine/World.h
        Public/Engine/Entity.h
        Public/Transform.h
        Public/Components/CameraComponent.hpp
        Public/Components/SpriteRendererComponent.hpp
        Public/Engine/Sprite.hpp
        Public/Renderer/GameRenderer.hpp
        Public/Engine/Service.hpp

        PRIVATE
        Private/Renderer.cpp
        Private/RingBuffer.cpp
        Private/Renderer/VkUtil.cpp
        Private/Renderer/Pipeline.cpp
        Private/Renderer/Swapchain.cpp
        Private/Renderer/Shader.cpp
        Private/Renderer/Buffer.cpp
        Private/Renderer/Image.cpp
        Private/Platform/Filesystem.cpp
        Private/Platform/Window.cpp
        Private/Renderer/RenderCtx.cpp
        Private/Engine/Engine.cpp
        Private/Engine/World.cpp
        Private/Engine/Entity.cpp
        Private/Transform.cpp
        Private/Components/CameraComponent.cpp
        Private/Components/SpriteRendererComponent.cpp
        Private/Engine/Sprite.cpp
        Private/Renderer/GameRenderer.cpp
        Private/Logging.cpp
)

target_compile_definitions(VeeRuntime PUBLIC
        VULKAN_HPP_NO_EXCEPTIONS
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VMA_STATIC_VULKAN_FUNCTIONS=0
        VMA_DYNAMIC_VULKAN_FUNCTIONS=1
)
if (WIN32)
    target_compile_definitions(VeeRuntime PUBLIC VK_USE_PLATFORM_WIN32_KHR)
endif ()
