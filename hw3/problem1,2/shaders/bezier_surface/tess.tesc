#version 410 core

layout(vertices = 16) out;

// outer02, outer13, inner0, inner1
uniform vec4 tessLevel;


void main()
{
    gl_TessLevelOuter[0] = tessLevel[0];
    gl_TessLevelOuter[1] = tessLevel[1];
    gl_TessLevelOuter[2] = tessLevel[0];
    gl_TessLevelOuter[3] = tessLevel[1];

    gl_TessLevelInner[0] = tessLevel[2];
    gl_TessLevelInner[1] = tessLevel[3];
    
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
