cmake_minimum_required(VERSION 3.28.0)

set(SOURCES
        Private/HelloTriangle.cpp
)

add_executable(HelloTriangle ${SOURCES})
target_include_directories(HelloTriangle PUBLIC Public/ PRIVATE Private/)
target_link_libraries(HelloTriangle PRIVATE VeeRuntime glfw EnTT::EnTT)

# Compile Shaders to spv
set(SHADER_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Resources)
set(SHADER_BINARY_PATH ${CMAKE_CURRENT_BINARY_DIR}/Resources)

file(MAKE_DIRECTORY ${SHADER_BINARY_PATH})
set(SHADERS
        Resources/color.frag
        Resources/entity.vert
        Resources/texture.frag
)
foreach (SHADER IN LISTS SHADERS)
    get_filename_component(FILENAME ${SHADER} NAME)
    add_custom_command(
            COMMAND glslc ${SHADER_SOURCE_PATH}/${FILENAME} -o ${SHADER_BINARY_PATH}/${FILENAME}.spv
            OUTPUT ${SHADER_BINARY_PATH}/${FILENAME}.spv
            DEPENDS ${SHADER}
            COMMENT "Building SPIR-V object ${SHADER_SOURCE_PATH}/${FILENAME}"
    )
    list(APPEND SPV_SHADERS ${SHADER_BINARY_PATH}/${FILENAME}.spv)
endforeach ()
add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})

# Copy textures to resources
set(TEXTURE_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Resources)
set(TEXTURE_BINARY_PATH ${CMAKE_CURRENT_BINARY_DIR}/Resources)

file(MAKE_DIRECTORY ${TEXTURE_BINARY_PATH})
set(TEXTURES
        cool.png
        cool2.png
)
foreach (TEXTURE IN LISTS TEXTURES)
    add_custom_command(
            OUTPUT ${TEXTURE_BINARY_PATH}/${TEXTURE}
            COMMAND ${CMAKE_COMMAND} -E copy
            ${TEXTURE_SOURCE_PATH}/${TEXTURE}
            ${TEXTURE_BINARY_PATH}/${TEXTURE}
            DEPENDS ${TEXTURE_SOURCE_PATH}/${TEXTURE}
    )
    list(APPEND OUT_TEXTURES ${TEXTURE_BINARY_PATH}/${TEXTURE})
endforeach ()

add_custom_target(textures ALL DEPENDS ${OUT_TEXTURES})

add_dependencies(HelloTriangle shaders textures)
