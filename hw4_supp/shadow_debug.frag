#version 330 core
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D depthSampler;
uniform vec2 nearFar;


float linearize(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * nearFar.x * nearFar.y)
        / (nearFar.y + nearFar.x - z * (nearFar.y - nearFar.x));
}

void main()
{
    float depth = texture(depthSampler, TexCoord).r;
    depth = linearize(depth) / nearFar.y;
    FragColor = vec4(vec3(depth), 1.0);
}
