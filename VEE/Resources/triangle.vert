#version 460

layout (location = 0) out vec4 outColor;
layout (push_constant) uniform Time
{
    float time;
};

const float PI = 3.1415926535897932384626433832795;
const float third = 2*PI/3;
float positions[3] = float[](
2*third,
third,
0
);

vec3 colors[3] = vec3[](
vec3(1.0, 0.0, 0.0),
vec3(0.0, 1.0, 0.0),
vec3(0.0, 0.0, 1.0)
);

void main() {
    gl_Position = vec4(sin(positions[gl_VertexIndex] + time) * .5, cos(positions[gl_VertexIndex] + time) * .5, 0.0, 1.0);
    outColor = vec4(colors[gl_VertexIndex], 1.0);
}