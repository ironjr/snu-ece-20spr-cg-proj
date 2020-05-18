#version 330 core
layout (location = 0) in vec3 aPos;

out vec4 FragPos;

uniform mat4 world;
uniform mat4 lightSpace;


void main()
{
    gl_Position = lightSpace * world * vec4(aPos, 1.0);
    FragPos = gl_Position;
}
