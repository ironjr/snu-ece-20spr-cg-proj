#version 330 core
in vec3 TexCoord;

out vec4 FragColor;

uniform float mixratio;
uniform samplerCube cubemap1;
uniform samplerCube cubemap2;


void main()
{   
    // mix two texture
    FragColor = mix(
        texture(cubemap1, TexCoord),
        texture(cubemap2, TexCoord),
        mixratio
    );
}
