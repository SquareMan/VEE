#version 460

layout (push_constant) uniform Time
{
    float time;
};
layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUV;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec2 outUV;

void main() {
    gl_Position = vec4(sin(inPosition.x + time) * .5, cos(inPosition.x + time) * .5, 0.0, 1.0);

    outColor = vec4(inColor, 1.0);
    outUV = inUV;
}