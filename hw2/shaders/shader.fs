#version 330 core
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex;


void main()
{
    // Ignore transparent texture pixel by 
    FragColor = texture(tex, TexCoord);
    if (FragColor.a < 0.5)
    {
        discard;
    }
}
