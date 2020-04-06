#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out VS_OUT
{
    uint nFace;
    vec3 vColor;
} vs_out;


void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    vs_out.nFace = uint(aPos.z);
    vs_out.vColor = aColor;
}
