#version 410 core

layout(location = 0) in vec3 aPos;

out VsOut
{
    vec3 vColor;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    vs_out.vColor = vec3(1.0f, 0.0f, 0.0f);
}
