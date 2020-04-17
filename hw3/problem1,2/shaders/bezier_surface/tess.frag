#version 410 core

// in vec3 vNormal;
in vec3 fColor;

out vec4 FragColor;


void main()
{   
    // vec3 normal = normalize(vNormal);
    // float intensity = abs(normal.z);
    //
    // fColor *= intensity;
    FragColor = vec4(fColor, 1.0);
}
