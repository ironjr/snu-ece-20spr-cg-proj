#version 330 core
in vec4 FragPos;

out vec4 FragColor;


void main()
{
    // gl_FragDepth = gl_FragCoord.z;
    float depth = gl_FragCoord.z;

    float moment1 = depth;

    float dx = dFdx(depth);
    float dy = dFdy(depth);
    float moment2 = depth * depth + 0.25 * (dx * dx + dy * dy);
    FragColor = vec4(moment1, moment2, 0.0, 0.0);
}
