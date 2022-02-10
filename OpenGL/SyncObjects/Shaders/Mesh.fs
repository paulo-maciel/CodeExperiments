#version 450 core
precision mediump float;

in OutputVS {
  vec3 normal;
  vec2 uv;
} outVS;

uniform vec3 baseColor;
uniform sampler2D diffuseTexture;
uniform vec3 lightDir;

layout(location = 0) out vec4 outColor;

void main() {
  vec3 diffuse = baseColor * texture(diffuseTexture, outVS.uv).rgb;
  vec3 ambient = vec3(0.25) * diffuse;
  float nDotL = max(0.0, dot(outVS.normal, lightDir));
  outColor = vec4(ambient + (diffuse * nDotL), 1.0);
}