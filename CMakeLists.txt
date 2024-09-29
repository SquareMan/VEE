cmake_minimum_required(VERSION 3.5.0)
project(VEE VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED SPIRV-Tools)
include_directories(${Vulkan_INCLUDE_DIR})

include(FetchContent)

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

link_libraries(glfw)

set(SOURCES
        VEE/Source/VEE.cpp
        VEE/Source/Renderer.cpp
        VEE/Source/Renderer.hpp
        VEE/Source/Platform/Window.cpp
        VEE/Source/Platform/Window.hpp
        VEE/Source/Platform/Win32/WindowHandle.hpp
        VEE/Source/Platform/Filesystem.cpp
        VEE/Source/Platform/Filesystem.hpp
)

add_executable(VEE ${SOURCES})
target_include_directories(VEE PRIVATE VEE/Source/)

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
            COMMAND glslc ${CMAKE_SOURCE_DIR}/VEE/Resources/${FILENAME} -o ${CMAKE_CURRENT_BINARY_DIR}/Resources/${FILENAME}.spv
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Resources/${FILENAME}.spv
            DEPENDS ${shader}
            COMMENT "Compiling ${FILENAME}"
    )
    list(APPEND SPV_SHADERS ${CMAKE_CURRENT_BINARY_DIR}/Resources/${FILENAME}.spv)
endforeach ()
add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})
add_dependencies(VEE shaders)
