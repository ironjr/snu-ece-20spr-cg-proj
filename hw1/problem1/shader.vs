#version 330 core
layout (location = 0) in vec3 aPos;

// declare uniform variables
uniform vec3 transform;

void main()
{
    // Fill in the blank
    float rot;
    if (aPos.x != 0 && aPos.y != 0)
        rot = atan(aPos.x, aPos.y);
    else
        rot = 0;
    rot += transform.z;

    float r = sqrt(aPos.x * aPos.x + aPos.y * aPos.y);
    gl_Position = vec4(
        r * cos(rot) + transform.x,
        r * sin(rot) + transform.y,
        aPos.z,
        1.0
    );
}
