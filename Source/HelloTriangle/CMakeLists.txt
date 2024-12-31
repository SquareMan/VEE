cmake_minimum_required(VERSION 3.28.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
        Private/HelloTriangle.cpp
)

add_executable(HelloTriangle ${SOURCES})
target_include_directories(HelloTriangle PUBLIC Public/ PRIVATE Private/)
target_link_libraries(HelloTriangle PRIVATE VeeRuntime VeeEditor glfw EnTT::EnTT)

# Compile Shaders to spv
set(SHADER_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Resources)
set(SHADER_BINARY_PATH ${CMAKE_CURRENT_BINARY_DIR}/Resources)

file(MAKE_DIRECTORY ${SHADER_BINARY_PATH})
file(GLOB shaders CONFIGURE_DEPENDS
        ${SHADER_SOURCE_PATH}/*.frag
        ${SHADER_SOURCE_PATH}/*.vert
)
foreach (shader IN LISTS shaders)
    get_filename_component(FILENAME ${shader} NAME)
    add_custom_command(
            COMMAND glslc ${SHADER_SOURCE_PATH}/${FILENAME} -o ${SHADER_BINARY_PATH}/${FILENAME}.spv
            OUTPUT ${SHADER_BINARY_PATH}/${FILENAME}.spv
            DEPENDS ${shader}
            COMMENT "Compiling ${FILENAME}"
    )
    list(APPEND SPV_SHADERS ${SHADER_BINARY_PATH}/${FILENAME}.spv)
endforeach ()
add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})
add_dependencies(HelloTriangle shaders)
