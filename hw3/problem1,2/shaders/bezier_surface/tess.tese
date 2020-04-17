#version 410 core

layout(quads, equal_spacing, ccw) in;

out TesOut
{
    // vec3 vNormal;
    vec3 fColor;
} tes_out;


void main()
{
    // Bernstein basis matrix is symmetric.
    mat4 bernsteinBasisMat = mat4(
        vec4(-1.0,  3.0, -3.0, 1.0),
        vec4( 3.0, -6.0,  3.0, 0.0),
        vec4(-3.0,  3.0,  0.0, 0.0),
        vec4( 1.0,  0.0,  0.0, 0.0)
    );
    // mat4 bernsteinBasisMat = mat4(

    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    float usq = u * u;
    float ucb = u * usq;
    float vsq = v * v;
    float vcb = v * vsq;
    vec4 uVec = vec4(ucb, usq, u, 1.0);
    vec4 vVec = vec4(vcb, vsq, v, 1.0);

    tes_out.fColor = mix(
        mix(
            vec3(1.0, 0.0, 0.0),
            vec3(0.0, 0.0, 1.0),
            u
        ),
        mix(
            vec3(1.0, 1.0, 0.0),
            vec3(0.0, 1.0, 1.0),
            u
        ),
        v
    );
    for (int i = 0; i < 4; ++i)
    {
        mat4 positionsMat = mat4(
            gl_in[0].gl_Position[i],
            gl_in[1].gl_Position[i],
            gl_in[2].gl_Position[i],
            gl_in[3].gl_Position[i],
            gl_in[4].gl_Position[i],
            gl_in[5].gl_Position[i],
            gl_in[6].gl_Position[i],
            gl_in[7].gl_Position[i],
            gl_in[8].gl_Position[i],
            gl_in[9].gl_Position[i],
            gl_in[10].gl_Position[i],
            gl_in[11].gl_Position[i],
            gl_in[12].gl_Position[i],
            gl_in[13].gl_Position[i],
            gl_in[14].gl_Position[i],
            gl_in[15].gl_Position[i]
        );

        gl_Position[i] = dot(
            uVec,
            bernsteinBasisMat * positionsMat * bernsteinBasisMat * vVec
        );
    }
}
