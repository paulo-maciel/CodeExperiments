#version 450 core

layout (triangles) in;
layout (fillmode, max_vertices=3) out;
in OutputVS
{
    vec2 uv;
    float depth;
} vert[];
out OutputVS {
    vec2 uv;
    float depth;
} outVS;

uniform mat4 reprojectionMatrix;

// gl_Position must contain unprojected point, NO_REPROJECTION define in ReprojectionMeshForwardTessEval shader

void main()
{
    float maxDepth = -1.0;
    for(int i = 0; i < gl_in.length(); i++)
    {
        if(vert[i].depth < 1.0)
        {
            maxDepth = max(maxDepth, vert[i].depth);
        }
    }
    if(maxDepth > -1.0)
    {
        for(int i = 0; i < gl_in.length(); i++)
        {
            if(vert[i].depth == 1.0)
            {
                vec4 v = gl_in[i].gl_Position;
                v.z = maxDepth;
                gl_Position = reprojectionMatrix * v;
            }
            else
            {
                gl_Position = reprojectionMatrix * gl_in[i].gl_Position;
            }
            
            outVS.uv = vert[i].uv;
            outVS.depth = vert[i].depth;
            
            EmitVertex();
        }
        EndPrimitive();
    }
}
