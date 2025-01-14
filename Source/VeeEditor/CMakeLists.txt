cmake_minimum_required(VERSION 3.28.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MODULES
        Public/Editor.ixx
)

set(PUBLIC_HEADERS
)

set(PRIVATE_HEADERS
        Private/ImguiRenderer.hpp
)

set(SOURCES
        Private/EditorApplication.cpp
        Private/ImguiRenderer.cpp
)

add_library(VeeEditor STATIC)
target_include_directories(VeeEditor PUBLIC ${glfw_SOURCE_DIR}/include)
target_link_libraries(VeeEditor VeeRuntime imgui glm)
target_include_directories(VeeEditor PRIVATE Private/)
target_sources(VeeEditor
        PUBLIC
        FILE_SET CXX_MODULES
        FILES
        ${MODULES}

        FILE_SET public_headers TYPE HEADERS
        BASE_DIRS Public/
        FILES
        ${PUBLIC_HEADERS}

        PRIVATE
        FILE_SET private_headers TYPE HEADERS
        BASE_DIRS Private/
        FILES
        ${PRIVATE_HEADERS}

        PRIVATE
        ${SOURCES}
)
