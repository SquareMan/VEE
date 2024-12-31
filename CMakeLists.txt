cmake_minimum_required(VERSION 3.28.0)
project(VEE VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED SPIRV-Tools)

include(FetchContent)

# VMA HPP
FetchContent_Declare(
        VulkanMemoryAllocator-Hpp
        GIT_REPOSITORY https://github.com/YaaZ/VulkanMemoryAllocator-Hpp.git
        GIT_TAG v3.1.0
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(VulkanMemoryAllocator-Hpp)

# vk_bootstrap
FetchContent_Declare(
        vk_bootstrap
        GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
        GIT_TAG v1.3.296
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(vk_bootstrap)

# glfw
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
        GIT_SHALLOW TRUE
)
if (NOT glfw_POPULATED)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Populate(glfw)
endif ()

add_subdirectory(${glfw_SOURCE_DIR})

# glm
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)

# imgui
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.91.4-docking
        GIT_SHALLOW TRUE
)
FetchContent_GetProperties(imgui)
if (NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
    add_Library(imgui STATIC
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

# entt
FetchContent_Declare(
        entt
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.14.0
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(entt)

# Boost (log)
set(BOOST_INCLUDE_LIBRARIES log log_setup)
set(BOOST_ENABLE_CMAKE ON)
#FetchContent_Declare(
#        Boost
#        GIT_REPOSITORY https://github.com/boostorg/boost.git
#        GIT_TAG boost-1.87.0
#        GIT_SHALLOW TRUE
#)
FetchContent_Declare(
        Boost
        URL https://github.com/boostorg/boost/releases/download/boost-1.87.0/boost-1.87.0-cmake.7z # downloading a zip release speeds up the download
        USES_TERMINAL_DOWNLOAD TRUE
        GIT_PROGRESS TRUE
        DOWNLOAD_NO_EXTRACT FALSE
)
FetchContent_MakeAvailable(Boost)

# magic_enum
FetchContent_Declare(
        magic_enum
        GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
        GIT_TAG v0.9.7
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(magic_enum)

add_subdirectory(Source/HelloTriangle)
add_subdirectory(Source/VeeEditor)
add_subdirectory(Source/VeeRuntime)