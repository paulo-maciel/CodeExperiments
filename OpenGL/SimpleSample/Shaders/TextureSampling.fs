#version 450 core
precision mediump float;

in OutputVS {
    vec2 uv;
    float depth;
} outVS;

uniform sampler2D diffuseTexture;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(diffuseTexture, outVS.uv);
}