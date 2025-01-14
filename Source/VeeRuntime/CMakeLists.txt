cmake_minimum_required(VERSION 3.28.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(VeeRuntime)
# I don't know why this isn't brought in from linking to the library
target_include_directories(VeeRuntime PUBLIC ${VulkanMemoryAllocator-Hpp_SOURCE_DIR}/VulkanMemoryAllocator/include)
target_link_libraries(VeeRuntime
        PUBLIC
        Vulkan-Hpp-Module
        VulkanMemoryAllocator-Hpp-Module
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
        FILE_SET CXX_MODULES FILES
        Public/Engine.ixx
        Public/Logging.ixx
        Public/Components/CameraComponent.ixx
        Public/Components/SpriteRendererComponent.ixx
        Public/Renderer.ixx
        Public/Renderer/Buffer.ixx
        Public/Renderer/GameRenderer.ixx
        Public/Renderer/Image.ixx
        Public/Renderer/IRenderer.ixx
        Public/Renderer/Pipeline.ixx
        Public/Renderer/RenderCtx.ixx
        Public/Renderer/Shader.ixx
        Public/Renderer/Swapchain.ixx
        Public/Renderer/VkUtil.ixx

        FILE_SET HEADERS
        BASE_DIRS Public/
        FILES
        Public/RingBuffer.hpp
        Public/Vertex.hpp
        Public/VeeCore.hpp
        Public/Platform/Filesystem.hpp
        Public/Platform/Window.hpp
        Public/Platform/WindowHandle.hpp
        Public/Engine/Sprite.hpp
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
        Private/Transform.cpp
        Private/Components/CameraComponent.cpp
        Private/Components/SpriteRendererComponent.cpp
        Private/Engine/Sprite.cpp
        Private/Renderer/GameRenderer.cpp
        Private/Logging.cpp
)

