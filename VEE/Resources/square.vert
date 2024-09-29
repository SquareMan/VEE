#version 460

layout (location = 0) out vec4 outColor;
layout (push_constant) uniform Time
{
    float time;
};
vec2 positions[] = vec2[](
vec2(-0.25, -0.25),
vec2(0.25, -0.25),
vec2(0.25, 0.25),
vec2(-0.25, 0.25)
);

vec3 colors[] = vec3[](
vec3(1.0, 0.0, 0.0),
vec3(0.0, 1.0, 0.0),
vec3(1.0, 0.0, 0.0),
vec3(0.0, 1.0, 0.0)
);

void main() {
    float x_off = (fract(time) - 0.5) * 2.0;
    gl_Position = vec4(positions[gl_VertexIndex].r + x_off, positions[gl_VertexIndex].g + sin(time*2.0), 0.0, 1.0);
    outColor = vec4(colors[gl_VertexIndex], 1.0);
}