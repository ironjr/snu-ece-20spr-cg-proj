#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

out VS_OUT
{
    vec2 TexCoord;
    vec3 SurfaceNormal;
    vec3 FragPos;
    vec4 FragPosLightSpace;
    mat3 TBN;
} vs_out;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpace;

uniform float useNormalMap;


void main()
{
    mat3 normalMat = mat3(transpose(inverse(world)));
	vs_out.TexCoord = aTexCoord;
    vs_out.SurfaceNormal = normalize(normalMat * aNormal);
    vs_out.FragPos = vec3(world * vec4(aPos, 1.0));
    vs_out.FragPosLightSpace = lightSpace * vec4(vs_out.FragPos, 1.0);
	gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);

	// on-off by key 1 (useNormalMap).
    // if model does not have a normal map, this should be always 0.
    // if useNormalMap is 0, we use a geometric normal as a surface normal.
    // if useNormalMap is 1, we use a geometric normal altered by normal map as a surface normal.
    vs_out.TBN = mat3(1.0);
	if (useNormalMap > 0.5)
    {
        vec3 N = vs_out.SurfaceNormal;
        vec3 T = normalize(normalMat * aTangent);
        T = normalize(T - dot(T, N) * N);
        vec3 B = cross(N, T);
        vs_out.TBN = mat3(T, B, N);
	}
}
