#version 330 core
// TODO: define in/out and uniform variables.
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex;


void main()
{
    // fill in
    // Hint) you can ignore transparent texture pixel by 
    // if(color.a < 0.5){discard;}
    FragColor = texture(tex, TexCoord);
}
