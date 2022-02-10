#version 450 core

layout (triangles) in;
layout (line_strip, max_vertices=3) out; // for wireframe

out OutputVS {
    vec2 uv;
    float depth;
} outVS;

in OutputVS
{
    vec2 uv;
    float depth;
} vert[];

void main()
{
    for(int i = 0; i < gl_in.length(); i++)
    {
        outVS.uv = vert[i].uv;
        outVS.depth = vert[i].depth;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();
}