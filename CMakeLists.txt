cmake_minimum_required(VERSION 3.28.0)
project(VEE VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# download CPM.cmake
file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.5/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
        EXPECTED_HASH SHA256=c46b876ae3b9f994b4f05a4c15553e0485636862064f1fcc9d8b4f832086bc5d
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)


# ---------------- DEPENDENCIES ------------------------
# Vulkan SDK should be installed on the system
find_package(Vulkan REQUIRED SPIRV-Tools)

CPMAddPackage("gh:GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator@3.1.0")
#CPMAddPackage("gh:YaaZ/VulkanMemoryAllocator-Hpp@3.1.0")
# vma::detail::resultCheck is missing an inline specifier in the current release, temporarily use this repo with the fix
CPMAddPackage(
        NAME VulkanMemoryAllocator-Hpp
        VERSION 3.1.0
        GIT_REPOSITORY https://github.com/walkiewicz125/VulkanMemoryAllocator-Hpp.git
        GIT_TAG abc1f44d51b9de65ebe86cb8051a1c2cfd914e5b
        OPTIONS "VMA_BUILD_CXX_MODULE ON" "VMA_BUILD_WITH_VULKAN_MODULE ON"
)

if (WIN32)
    target_compile_definitions(Vulkan-Hpp-Module PUBLIC VK_USE_PLATFORM_WIN32_KHR)
endif ()
target_compile_definitions(Vulkan-Hpp-Module PUBLIC
        VULKAN_HPP_NO_EXCEPTIONS
        VULKAN_HPP_RAII_NO_EXCEPTIONS
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
)
target_compile_features(Vulkan-Hpp-Module PUBLIC cxx_std_23)
target_compile_definitions(VulkanMemoryAllocator-Hpp-Module PUBLIC
        VMA_STATIC_VULKAN_FUNCTIONS=0
        VMA_DYNAMIC_VULKAN_FUNCTIONS=1
)
target_compile_features(VulkanMemoryAllocator-Hpp-Module PUBLIC cxx_std_23)
CPMAddPackage("gh:charles-lunarg/vk-bootstrap@1.3.296")
CPMAddPackage(
        NAME glfw
        GITHUB_REPOSITORY glfw/glfw
        GIT_TAG 3.3.8
        OPTIONS
        "GLFW_BUILD_DOCS OFF CACHE BOOL \"\" FORCE"
        "GLFW_BUILD_TESTS OFF CACHE BOOL \"\" FORCE"
        "GLFW_BUILD_EXAMPLES OFF CACHE BOOL \"\" FORCE"
)
CPMAddPackage("gh:g-truc/glm#1.0.1")
CPMAddPackage("gh:ocornut/imgui@1.91.4-docking")
CPMAddPackage("gh:skypjack/entt@3.14.0")
CPMAddPackage(
        NAME Boost
        VERSION 1.87.0
        URL https://github.com/boostorg/boost/releases/download/boost-1.87.0/boost-1.87.0-cmake.7z # downloading a zip release speeds up the download
        URL_HASH SHA256=61d61a3f33283bab8f3736745a4e47c37141f07e413986cd05d1f942c69d529a
        OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_INCLUDE_LIBRARIES log"
)
CPMAddPackage("gh:Neargye/magic_enum@0.9.7")

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
    target_include_directories(imgui PRIVATE ${Vulkan_INCLUDE_DIR})
    target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
    target_compile_definitions(imgui PUBLIC IMGUI_IMPL_VULKAN_NO_PROTOTYPES)
endif ()

add_subdirectory(Source/HelloTriangle)
add_subdirectory(Source/VeeEditor)
add_subdirectory(Source/VeeRuntime)