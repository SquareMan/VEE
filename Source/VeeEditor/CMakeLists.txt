cmake_minimum_required(VERSION 3.13.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
        Source/EditorApplication.cpp
        Source/EditorApplication.hpp
)

add_library(VeeEditor STATIC ${SOURCES})
target_include_directories(VeeEditor PUBLIC ${glfw_SOURCE_DIR}/include)
target_link_libraries(VeeEditor VeeRuntime)
# TODO: Separate public and private include paths
target_include_directories(VeeEditor PUBLIC Source/)
