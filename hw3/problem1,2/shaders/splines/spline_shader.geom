#version 410 core

#define N 20

subroutine mat4 splineInterpType();
subroutine uniform splineInterpType splineInterp;

layout (lines_adjacency) in;
layout (line_strip, max_vertices = N) out;

in vec3 vPos[];

out vec3 fColor;


subroutine(splineInterpType)
mat4 interpBezier()
{
    mat4 bernsteinBasisMat = mat4(
        vec4(-1.0,  3.0, -3.0, 1.0),
        vec4( 3.0, -6.0,  3.0, 0.0),
        vec4(-3.0,  3.0,  0.0, 0.0),
        vec4( 1.0,  0.0,  0.0, 0.0)
    );
    return bernsteinBasisMat;
}


subroutine(splineInterpType)
mat4 interpBSpline()
{
    mat4 bSplineBasisMat = mat4(
        vec4(-0.16666667,  0.5, -0.5, 0.16666667),
        vec4( 0.5,        -1.0,  0.0, 0.66666667),
        vec4(-0.5,         0.5,  0.5, 0.16666667),
        vec4( 0.16666667,  0.0,  0.0, 0.0)
    );
    return bSplineBasisMat;
}


subroutine(splineInterpType)
mat4 interpCatmullRomUniform()
{
    // Simplest Catmull-Rom interpolation
    // \alpha = 0 where t_{i} = t_{i-1} + dist(p_{i}, p_{i-1})^\alpha
    mat4 catmullRomBasisMat = mat4(
        vec4(-0.5,  1.0, -0.5, 0.0),
        vec4( 1.5, -2.5,  0.0, 1.0),
        vec4(-1.5,  2.0,  0.5, 0.0),
        vec4( 0.5, -0.5,  0.0, 0.0)
    );
    return catmullRomBasisMat;
}


subroutine(splineInterpType)
mat4 interpCatmullRomCentripetal()
{
    // \alpha = 0.5 where t_{i} = t_{i-1} + dist(p_{i}, p_{i-1})^\alpha
    mat4 constConversionMat = mat4(
        vec4( 2.0, -3.0, 0.0, 1.0),
        vec4(-2.0,  3.0, 0.0, 0.0),
        vec4( 1.0, -2.0, 1.0, 0.0),
        vec4( 1.0, -1.0, 0.0, 0.0)
    );

    // vec3 pos0Vec = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
    // vec3 pos1Vec = gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w;
    // vec3 pos2Vec = gl_in[2].gl_Position.xyz / gl_in[2].gl_Position.w;
    // vec3 pos3Vec = gl_in[3].gl_Position.xyz / gl_in[3].gl_Position.w;
    vec3 pos0Vec = vPos[0];
    vec3 pos1Vec = vPos[1];
    vec3 pos2Vec = vPos[2];
    vec3 pos3Vec = vPos[3];
    float d1 = sqrt(distance(pos0Vec, pos1Vec));
    float d2 = sqrt(distance(pos1Vec, pos2Vec));
    float d3 = sqrt(distance(pos2Vec, pos3Vec));

    float temp1;
    float temp2;
    temp1 = d2 / (d1 + d2);
    temp2 = d2 / d1;
    vec4 conversion1Vec = vec4(
        temp1 - temp2,
        temp2 - 1.0,
        1.0 - temp1,
        0.0
    );
    temp1 = d2 / d3;
    temp2 = d2 / (d2 + d3);
    vec4 conversion2Vec = vec4(
        0.0,
        temp2 - 1.0,
        1.0 - temp1,
        temp1 - temp2
    );
    mat4 varConversionMat = transpose(mat4(
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        conversion1Vec,
        conversion2Vec
    ));

    mat4 catmullRomBasisMat = constConversionMat * varConversionMat;
    return catmullRomBasisMat;
}


subroutine(splineInterpType)
mat4 interpCatmullRomChordal()
{
    // \alpha = 1.0 where t_{i} = t_{i-1} + dist(p_{i}, p_{i-1})^\alpha
    mat4 constConversionMat = mat4(
        vec4( 2.0, -3.0, 0.0, 1.0),
        vec4(-2.0,  3.0, 0.0, 0.0),
        vec4( 1.0, -2.0, 1.0, 0.0),
        vec4( 1.0, -1.0, 0.0, 0.0)
    );

    // vec3 pos0Vec = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
    // vec3 pos1Vec = gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w;
    // vec3 pos2Vec = gl_in[2].gl_Position.xyz / gl_in[2].gl_Position.w;
    // vec3 pos3Vec = gl_in[3].gl_Position.xyz / gl_in[3].gl_Position.w;
    vec3 pos0Vec = vPos[0];
    vec3 pos1Vec = vPos[1];
    vec3 pos2Vec = vPos[2];
    vec3 pos3Vec = vPos[3];
    float d1 = distance(pos0Vec, pos1Vec);
    float d2 = distance(pos1Vec, pos2Vec);
    float d3 = distance(pos2Vec, pos3Vec);

    float temp1;
    float temp2;
    temp1 = d2 / (d1 + d2);
    temp2 = d2 / d1;
    vec4 conversion1Vec = vec4(
        temp1 - temp2,
        temp2 - 1.0,
        1.0 - temp1,
        0.0
    );
    temp1 = d2 / d3;
    temp2 = d2 / (d2 + d3);
    vec4 conversion2Vec = vec4(
        0.0,
        temp2 - 1.0,
        1.0 - temp1,
        temp1 - temp2
    );
    mat4 varConversionMat = transpose(mat4(
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        conversion1Vec,
        conversion2Vec
    ));

    mat4 catmullRomBasisMat = constConversionMat * varConversionMat;
    return catmullRomBasisMat;
}


void main()
{    
    mat4 positionsMat = transpose(mat4(
        gl_in[0].gl_Position,
        gl_in[1].gl_Position,
        gl_in[2].gl_Position,
        gl_in[3].gl_Position
    ));
    mat4 basisMat = splineInterp();

    for (int i = 0; i < N; ++i)
    {
        float t = float(i) / (N - 1);
        float tsq = t * t;
        float tcb = tsq * t;
        vec4 tVec = vec4(tcb, tsq, t, 1.0);
        gl_Position = tVec * basisMat * positionsMat;
        fColor = mix(
            vec3(0.0f, 1.0f, 1.0f),
            vec3(1.0f, 1.0f, 0.0f),
            t
        );
        EmitVertex();
    }
    EndPrimitive();
}
