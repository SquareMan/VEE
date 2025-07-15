cmake_minimum_required(VERSION 3.28.0)

set(SOURCES
)

add_library(VeeEditor STATIC ${SOURCES})
target_sources(VeeEditor
        PUBLIC
        FILE_SET headers TYPE HEADERS
        BASE_DIRS Public/
        FILES
        Public/EditorApplication.hpp
        Public/EditorRenderPass.hpp

        PRIVATE
        FILE_SET private_headers TYPE HEADERS
        BASE_DIRS Private/
        FILES

        PRIVATE
        Private/EditorApplication.cpp
        Private/EditorRenderPass.cpp
)
target_include_directories(VeeEditor PUBLIC ${glfw_SOURCE_DIR}/include)
target_link_libraries(VeeEditor
        PUBLIC
        VeeRuntime

        PRIVATE
        imgui
        glm
        Tracy::TracyClient
)
target_include_directories(VeeEditor PUBLIC Public/ PRIVATE Private/)
