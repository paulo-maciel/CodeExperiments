#version 450 core

layout(location = 0) in highp vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 projViewWorld;
uniform mat4 viewWorld;

out OutputVS {
  vec3 normal;
  vec2 uv;
} outVS;

void main() {
  gl_Position = projViewWorld * vec4(position, 1.0);
  outVS.normal = normalize(mat3(viewWorld) * normal);
  outVS.uv = uv;
}