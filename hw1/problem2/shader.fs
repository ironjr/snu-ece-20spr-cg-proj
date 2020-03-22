#version 330 core
// declare input and output
in mediump vec3 verColor;

out mediump vec4 FragColor;

void main()
{
    // fill in
    FragColor = vec4(verColor, 1.0f);
}
