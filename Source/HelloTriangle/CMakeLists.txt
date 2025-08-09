cmake_minimum_required(VERSION 3.28.0)

set(SOURCES
        Private/HelloTriangle.cpp
)

add_executable(HelloTriangle ${SOURCES})
target_compile_options(HelloTriangle PRIVATE ${VEE_WARNING_FLAGS})
target_include_directories(HelloTriangle PUBLIC Public/ PRIVATE Private/)
target_link_libraries(HelloTriangle
        PRIVATE
        VeeRuntime
        Tracy::TracyClient
)

set(HELLO_TRIANGLE_CONTENT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Resources)
cmake_path(RELATIVE_PATH HELLO_TRIANGLE_CONTENT_PATH BASE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} OUTPUT_VARIABLE HELLO_TRIANGLE_CONTENT_PATH)
message(STATUS "HELLO_TRIANGLE_CONTENT_PATH: " ${HELLO_TRIANGLE_CONTENT_PATH})
target_compile_definitions(HelloTriangle PUBLIC HELLO_TRIANGLE_CONTENT_PATH=\"${HELLO_TRIANGLE_CONTENT_PATH}\")
