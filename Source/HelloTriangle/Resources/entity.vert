#version 460

layout (push_constant) uniform Mat {
    mat4 m;
    mat4 proj;
};
layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUV;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec2 outUV;

void main() {
    vec4 pos = vec4(inPosition.xy, 0, 1);
    gl_Position = proj * m * pos;

    outColor = vec4(inColor, 1.0);
    outUV = inUV;
}