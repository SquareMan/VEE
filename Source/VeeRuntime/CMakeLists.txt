cmake_minimum_required(VERSION 3.13.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
        Source/Renderer.cpp
        Source/Renderer.hpp
        Source/RingBuffer.cpp
        Source/RingBuffer.hpp
        Source/Vertex.hpp
        Source/Renderer/VkUtil.cpp
        Source/Renderer/VkUtil.hpp
        Source/Renderer/Pipeline.cpp
        Source/Renderer/Pipeline.hpp
        Source/Renderer/Shader.cpp
        Source/Renderer/Shader.hpp
        Source/Renderer/Swapchain.cpp
        Source/Renderer/Swapchain.hpp
        Source/Renderer/Buffer.cpp
        Source/Renderer/Buffer.hpp
        Source/Renderer/Image.cpp
        Source/Renderer/Image.hpp
        Source/VeeCore.hpp
        Source/Platform/Filesystem.cpp
        Source/Platform/Filesystem.hpp
        Source/Platform/Window.cpp
        Source/Platform/Window.hpp
        Source/Platform/Win32/WindowHandle.hpp
        Source/Engine.cpp
        Source/Engine.hpp
)

add_library(VeeRuntime STATIC ${SOURCES})
target_include_directories(VeeRuntime PUBLIC ${Vulkan_INCLUDE_DIR})
target_include_directories(VeeRuntime PUBLIC ${VulkanMemoryAllocator-Hpp_SOURCE_DIR}/include)
target_link_libraries(VeeRuntime glm::glm vk-bootstrap::vk-bootstrap imgui glfw)
# TODO: Separate public and private include paths
target_include_directories(VeeRuntime PUBLIC Source/)
