#version 330 core
struct Material
{
    sampler2D diffuseSampler;
    sampler2D specularSampler;
    sampler2D normalSampler;
    float shininess;
}; 

struct Light
{
    // vec3 position;
    vec3 direction;
    vec3 color; // this is I_d (I_s = I_d, I_a = 0.3 * I_d)
};

in VS_OUT
{
    vec2 TexCoord;
    vec3 SurfaceNormal;
    vec3 FragPos;
    vec4 FragPosLightSpace;
    mat3 TBN;
} fs_in;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Light light;
uniform Material material;

uniform sampler2D depthmapSampler;

uniform float useNormalMap;
uniform float useSpecularMap;
uniform float useShadow;
uniform float useLighting;


float random(vec4 seed4)
{
    float temp = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(temp) * 43758.5453);
}


void main()
{
	vec3 color = texture(material.diffuseSampler, fs_in.TexCoord).rgb;
    vec3 surfaceNormal = normalize(fs_in.SurfaceNormal);
    vec3 lightDir = normalize(light.direction);
    // vec3 lightDir = normalize(light.position - fs_in.FragPos);

    // On-off by key 3 (useLighting). 
    // If useLighting is 0, return diffuse value without considering any lighting.(DO NOT CHANGE)
	if (useLighting < 0.5)
    {
        FragColor = vec4(color, 1.0); 
        return; 
    }

    // On-off by key 2 (useShadow).
    // Calculate shadow.
    // If useShadow is 0, do not consider shadow.
    // If useShadow is 1, consider shadow.
    float shadow = 0.0;
    if (useShadow > 0.5)
    {
        vec3 proj = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
        proj = proj * 0.5 + 0.5;
        float currentDepth = proj.z;
        float bias = max(0.0025 * (1.0 - dot(surfaceNormal, lightDir)), 0.00025);

        // 1. Vanilla shadow.
        // float closestDepth = texture(depthmapSampler, proj.xy).r;
        // if (currentDepth < 1.0)
        // {
        //     shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
        // }

        // 2. Basic PCF.
        // vec2 texelSize = 1.0 / textureSize(depthmapSampler, 0);
        // for (int x = -1; x <= 1; ++x)
        // {
        //     for (int y = -1; y <= 1; ++y)
        //     {
        //         float pcfDepth = texture(depthmapSampler, proj.xy + vec2(x, y) * texelSize).r;
        //         shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        //     }
        // }
        // shadow *= 0.11111111;
        
        // 3. Stratified Poisson sampling PCF.
        vec2 poissonDisk[4] = vec2[](
            vec2(-0.94201624,  -0.39906216),
            vec2( 0.94558609,  -0.76890725),
            vec2(-0.094184101, -0.92938870),
            vec2( 0.34495938,   0.29387760)
        );
        for (int i = 0; i < 9; ++i)
        {
            // int index = int(16.0 * random(vec4(gl_FragCoord.xyy, i))) % 16;
            int index = int(16.0 * random(vec4(floor(fs_in.FragPos * 1000.0), i))) % 16;
            float pcfDepthPoisson = texture(
                depthmapSampler, proj.xy + poissonDisk[index] / 700.0
            ).r;
            shadow += currentDepth - bias > pcfDepthPoisson ? 1.0 : 0.0;
        }
        shadow *= 0.11111111;

        // 4. Variance shadow mapping (VSM).
        // vec2 moments = texture(depthmapSampler, proj.xy).rg;
        // float E_x2 = moments.y;
        // float Ex_2 = moments.x * moments.x;
        // float varx = E_x2 - Ex_2;
        // varx = max(varx, 0.00005);
        // float mD = moments.x - currentDepth;
        // float mD_2 = mD * mD;
        // float p = varx / (varx + mD_2);
        // shadow = 1.0 - max(p, currentDepth - bias < moments.x ? 1.0 : 0.0);
    }

    // On-off by key 1 (useNormalMap).
    // If model does not have a normal map, this should be always 0.
    // If useNormalMap is 0, we use a geometric normal as a surface normal.
    // If useNormalMap is 1, we use a geometric normal altered by normal map as a surface normal.
	if (useNormalMap > 0.5)
	{
        surfaceNormal = texture(material.normalSampler, fs_in.TexCoord).rgb;
        surfaceNormal = surfaceNormal * 2.0 - 1.0;
        surfaceNormal = normalize(fs_in.TBN * surfaceNormal);
	}
	
    // If model does not have a specular map, this should be always 0.
    // If useSpecularMap is 0, ignore specular lighting.
    // If useSpecularMap is 1, calculate specular lighting.
    vec3 specularColor = vec3(0.0);
	if (useSpecularMap > 0.5)
	{
        //use only red channel of specularSampler as a reflectance coefficient(k_s).
        specularColor = texture(material.specularSampler, fs_in.TexCoord).rrr;
	}

    // Implement Phong illumination model.
    color = color * light.color;

    // 1. Ambiant color.
    vec3 ambiant = 0.3 * color;

    // 2. Diffuse color.
    float diffuseCosine = max(dot(surfaceNormal, -lightDir), 0.0);
    vec3 diffuse = diffuseCosine * color;
    
    // 3. Specular color.
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(lightDir, surfaceNormal);
    float specularCosine = max(dot(viewDir, reflectDir), 0.0);
    float specularFactor = pow(specularCosine, material.shininess);
    vec3 specular = specularCosine * light.color * specularColor;

    FragColor = vec4(ambiant + (1.0 - shadow) * (diffuse + specular), 1.0);
}
