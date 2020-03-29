#version 330 core
// TODO: define in/out and uniform variables.
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
