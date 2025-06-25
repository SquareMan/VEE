cmake_minimum_required(VERSION 3.28.0)
project(VEE VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_CXX_FLAGS "-freflection-latest -stdlib=libc++")

# download CPM.cmake
file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.41.0/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
        EXPECTED_HASH SHA256=e570f03806b9aae2082ca5b950a9e6b3b41ad56972a78a876aedcaad16653116
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)


# ---------------- DEPENDENCIES ------------------------
# Vulkan SDK should be installed on the system
find_package(Vulkan REQUIRED SPIRV-Tools)

#CPMAddPackage("gh:GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator@3.2.1")
CPMAddPackage("gh:YaaZ/VulkanMemoryAllocator-Hpp@3.2.1")
CPMAddPackage(
        URI "gh:charles-lunarg/vk-bootstrap@1.3.296"
        OPTIONS "VK_BOOTSTRAP_DISABLE_WARNINGS ON"
)
CPMAddPackage(
        NAME glfw
        GITHUB_REPOSITORY glfw/glfw
        GIT_TAG 3.3.8
        OPTIONS
        "GLFW_BUILD_DOCS OFF"
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_EXAMPLES OFF"
)
CPMAddPackage("gh:g-truc/glm#1.0.1")
CPMAddPackage("gh:ocornut/imgui@1.91.4-docking")
CPMAddPackage("gh:skypjack/entt@3.14.0")
CPMAddPackage("gh:Neargye/magic_enum@0.9.7")
CPMAddPackage("gh:nothings/stb#f056911")

set(TRACY_ENABLE OFF)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(TRACY_ENABLE ON)
endif ()
CPMAddPackage("gh:wolfpld/tracy@0.11.1")

if (stb_ADDED)
    add_library(stb INTERFACE)
    target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
endif ()

if (imgui_ADDED)
    add_library(imgui STATIC
            ${imgui_SOURCE_DIR}/imgui.cpp
            ${imgui_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_SOURCE_DIR}/imgui_demo.cpp
            ${imgui_SOURCE_DIR}/imgui_tables.cpp
            ${imgui_SOURCE_DIR}/imgui_widgets.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp)
    target_link_libraries(imgui PRIVATE glfw)
    target_include_directories(imgui PRIVATE ${Vulkan_INCLUDE_DIR} ${imgui_SOURCE_DIR})
    target_include_directories(imgui SYSTEM INTERFACE ${imgui_SOURCE_DIR})
    target_compile_definitions(imgui PUBLIC IMGUI_IMPL_VULKAN_NO_PROTOTYPES)
    target_compile_options(imgui PRIVATE
            $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
                -Wno-nontrivial-memcall>
    )
endif ()


add_subdirectory(Source/HelloTriangle)
add_subdirectory(Source/VeeEditor)
add_subdirectory(Source/VeeRuntime)
add_subdirectory(Source/VeeCore)
