#version 450 core

layout(location = 0) in highp vec2 position;

out OutputVS {
    vec2 uv;
    float depth;
} outVS;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    outVS.uv = 0.5 * position + 0.5;
    outVS.depth = 0.0;
}