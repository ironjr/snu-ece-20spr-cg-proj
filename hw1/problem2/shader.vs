#version 330 core
// declare input and output
in mediump vec3 position;
in mediump vec3 color;

out mediump vec3 verColor;

// declare uniform vairable
uniform mediump mat4 transform;

void main()
{    
    // fill in
    gl_Position = transform * vec4(position, 1.0);
    verColor = color;
}
