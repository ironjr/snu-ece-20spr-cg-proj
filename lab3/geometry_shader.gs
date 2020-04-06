#version 330 core
#define PI 3.141592
layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

in VS_OUT
{
    uint nFace;
    vec3 vColor;
} gs_in[];

out vec3 fColor;

void main()
{    
    int nFace = int(gs_in[0].nFace);
    fColor = gs_in[0].vColor;
    int j;
    for (int i = 1; i <= nFace; ++i)
    {
        j = i / 2;
        if (i % 2 == 1)
            j = -j;
        gl_Position = gl_in[0].gl_Position;
        gl_Position.x += 0.2 * cos(j * 2.0 * PI / nFace);
        gl_Position.y += 0.2 * sin(j * 2.0 * PI / nFace);
        EmitVertex();
    }
    EndPrimitive();
}
