#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skyboxSampler1;


void main()
{
    FragColor = vec4(texture(skyboxSampler1, TexCoords).rgb, 1.0f);
}
