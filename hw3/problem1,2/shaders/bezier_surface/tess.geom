#version 410 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in TesOut
{
    // vec3 vNormal;
    vec3 fColor;
} gs_in[];

// out vec3 vNormal;
out vec3 fColor;


void main()
{
    for (int i = 0; i < 3; ++i)
    {
        // vNormal = gs_in[i].vNormal;
        fColor = gs_in[i].fColor;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}
