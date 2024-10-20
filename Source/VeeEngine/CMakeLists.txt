cmake_minimum_required(VERSION 3.13.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(SOURCES
        Source/VeeCore.hpp
        Source/Platform/Filesystem.cpp
        Source/Platform/Filesystem.hpp
        Source/Platform/Window.cpp
        Source/Platform/Window.hpp
        Source/Platform/Win32/WindowHandle.hpp
)

add_library(VeeEngine STATIC ${SOURCES})
target_include_directories(VeeEngine PUBLIC ${glfw_SOURCE_DIR}/include)
target_link_libraries(VeeEngine glfw)
# TODO: Separate public and private include paths
target_include_directories(VeeEngine PUBLIC Source/)
