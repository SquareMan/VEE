cmake_minimum_required(VERSION 3.28.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
        Private/EditorApplication.cpp
        Public/EditorApplication.hpp
        Private/ImguiRenderer.cpp
        Private/ImguiRenderer.hpp
)

add_library(VeeEditor STATIC ${SOURCES})
target_include_directories(VeeEditor PUBLIC ${glfw_SOURCE_DIR}/include)
target_link_libraries(VeeEditor VeeRuntime imgui glm)
target_include_directories(VeeEditor PUBLIC Public/ PRIVATE Private/)
