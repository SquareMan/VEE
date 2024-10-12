cmake_minimum_required(VERSION 3.13.0)
project(VEE VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED SPIRV-Tools)
include_directories(${Vulkan_INCLUDE_DIR})

include(FetchContent)

FetchContent_Declare(
        VulkanMemoryAllocator-Hpp
        GIT_REPOSITORY https://github.com/YaaZ/VulkanMemoryAllocator-Hpp.git
        GIT_TAG v3.1.0
)
FetchContent_MakeAvailable(VulkanMemoryAllocator-Hpp)
include_directories(${VulkanMemoryAllocator-Hpp_SOURCE_DIR}/include)

include(FetchContent)
FetchContent_Declare(
        vk_bootstrap
        GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
        GIT_TAG v1.3.296
)
FetchContent_MakeAvailable(vk_bootstrap)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
)
if (NOT glfw_POPULATED)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Populate(glfw)
endif ()

add_subdirectory(${glfw_SOURCE_DIR})
include_directories(${glfw_SOURCE_DIR}/include)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)


set(SOURCES
        VEE/Source/VEE.cpp
        VEE/Source/Renderer.cpp
        VEE/Source/Renderer.hpp
        VEE/Source/Platform/Window.cpp
        VEE/Source/Platform/Window.hpp
        VEE/Source/Platform/Win32/WindowHandle.hpp
        VEE/Source/Platform/Filesystem.cpp
        VEE/Source/Platform/Filesystem.hpp
        VEE/Source/Renderer/VkUtil.cpp
        VEE/Source/Renderer/VkUtil.hpp
        VEE/Source/RingBuffer.cpp
        VEE/Source/RingBuffer.hpp
        VEE/Source/Renderer/Pipeline.cpp
        VEE/Source/Renderer/Pipeline.hpp
        VEE/Source/Renderer/Shader.cpp
        VEE/Source/Renderer/Shader.hpp
        VEE/Source/Vertex.hpp
        VEE/Source/Renderer/Swapchain.cpp
        VEE/Source/Renderer/Swapchain.hpp
        VEE/Source/Renderer/Buffer.cpp
        VEE/Source/Renderer/Buffer.hpp
        VEE/Source/Renderer/Image.cpp
        VEE/Source/Renderer/Image.hpp
)

add_executable(VEE ${SOURCES})
target_include_directories(VEE PRIVATE VEE/Source/)
target_link_libraries(VEE PRIVATE glfw glm::glm vk-bootstrap::vk-bootstrap)

# Compile Shaders to spv
set(SHADER_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/VEE/Resources)
set(SHADER_BINARY_PATH ${CMAKE_CURRENT_BINARY_DIR}/Resources)

file(MAKE_DIRECTORY ${SHADER_BINARY_PATH})
file(GLOB shaders CONFIGURE_DEPENDS
        ${SHADER_SOURCE_PATH}/*.frag
        ${SHADER_SOURCE_PATH}/*.vert
)
foreach (shader IN LISTS shaders)
    get_filename_component(FILENAME ${shader} NAME)
    add_custom_command(
            COMMAND glslc ${SHADER_SOURCE_PATH}/${FILENAME} -o ${SHADER_BINARY_PATH}/${FILENAME}.spv
            OUTPUT ${SHADER_BINARY_PATH}/${FILENAME}.spv
            DEPENDS ${shader}
            COMMENT "Compiling ${FILENAME}"
    )
    list(APPEND SPV_SHADERS ${SHADER_BINARY_PATH}/${FILENAME}.spv)
endforeach ()
add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})
add_dependencies(VEE shaders)
