cmake_minimum_required(VERSION 3.13.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
        Private/Renderer.cpp
        Public/Renderer.hpp
        Private/RingBuffer.cpp
        Public/RingBuffer.hpp
        Public/Vertex.hpp
        Private/Renderer/VkUtil.cpp
        Public/Renderer/VkUtil.hpp
        Private/Renderer/Pipeline.cpp
        Public/Renderer/Pipeline.hpp
        Private/Renderer/Shader.cpp
        Public/Renderer/Shader.hpp
        Private/Renderer/Swapchain.cpp
        Public/Renderer/Swapchain.hpp
        Private/Renderer/Buffer.cpp
        Public/Renderer/Buffer.hpp
        Private/Renderer/Image.cpp
        Public/Renderer/Image.hpp
        Public/VeeCore.hpp
        Private/Platform/Filesystem.cpp
        Public/Platform/Filesystem.hpp
        Private/Platform/Window.cpp
        Public/Platform/Window.hpp
        Public/Platform/WindowHandle.hpp
        Private/Engine.cpp
        Public/Engine.hpp
)

add_library(VeeRuntime STATIC ${SOURCES})
target_include_directories(VeeRuntime PUBLIC ${Vulkan_INCLUDE_DIR})
target_include_directories(VeeRuntime PUBLIC ${VulkanMemoryAllocator-Hpp_SOURCE_DIR}/include)
target_link_libraries(VeeRuntime glm::glm vk-bootstrap::vk-bootstrap imgui glfw)
target_include_directories(VeeRuntime PUBLIC Public/ PRIVATE Private/)
