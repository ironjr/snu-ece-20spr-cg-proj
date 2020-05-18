#version 330
#define PI 3.14159265

// Options for materials.
#define SCATTER_TYPE_PHONG         0
#define SCATTER_TYPE_LAMBERTIAN    1
#define SCATTER_TYPE_REFRACTIVE    2
#define SCATTER_TYPE_SPECULAR      3

// To prevent point too close to surface.
#define EPSILON 0.00001
// Softening the shadows.
#define SHADOW_JITTER 0.001

// Static environment setup.
#define NUM_SPHERES 4
#define NUM_BOXES 2
#define NUM_POINT_LIGHTS 3
#define NUM_AREA_LIGHTS 1

// Define the quality of the ray tracing.
// Maximum bounce.
#define MAX_DEPTH 4
// Number of samples per pixel.
#define NUM_SAMPLES 32
#define NUM_SAMPLES_SHADOW 8


struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct Material
{
    // Phong shading coefficients
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;

    // Reflect / Refract
    vec3 R0; // Schlick approximation
    float ior; // Index of refration (> 1)

    // For refractive material
    vec3 extinction_constant;
    vec3 shadow_attenuation_constant;

    // 0 : Phong + fresnel + ideal specular (mirror)
    // 1 : Lambertian
    // 2 : Refractive dielectric
    // 3 : Specular
    int scatter_type;
};

// Point light source
struct PointLight
{
    vec3 position;
    vec3 color;
    bool castShadow;
};

struct Triangle
{
    vec3 v0;
    vec3 v1;
    vec3 v2;
    // We didn't add material to triangle because it requires so many uniform
    // memory when we use mesh...
};

// Area light source
struct TriangleLight
{
    Triangle geom;
    vec3 color;
    bool castShadow;
};

// Hit information
struct HitRecord
{
    float t;        // Distance to hit point
    vec3 p;         // Hit point
    vec3 normal;    // Hit point normal
    Material mat;   // Hit point material
};

// Geometry
struct Sphere
{
    vec3 center;
    float radius;
    Material mat;
};

struct Plane
{
    vec3 normal;
    vec3 p0;
    Material mat;
};

struct Box
{
    vec3 bmin;
    vec3 bmax;
    Material mat;
};


in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 cameraPosition;
uniform mat3 cameraToWorldRotMatrix;
uniform float fovY; // Set this to 45.
uniform float H;
uniform float W;
uniform samplerCube environmentMap;
uniform vec3 ambientLightColor;
uniform Material material_ground;
uniform Material material_box;
uniform Material material_gold;
uniform Material material_dielectric_glass;
uniform Material material_mirror;
uniform Material material_lambert;
uniform Material material_mesh;

// Use this for mesh
layout (std140) uniform mesh_vertices_ubo {
    vec3 mesh_vertices[500];
};
layout (std140) uniform mesh_tri_indices_ubo {
    ivec3 mesh_tri_indices[500];
};
uniform int meshTriangleNumber;


Sphere spheres[] = Sphere[](
    Sphere(vec3( 1.0, 0.5,-1.0), 0.499, material_gold),
    Sphere(vec3(-1.0, 0.5,-1.0), 0.499, material_gold),
    Sphere(vec3( 0.0, 0.5, 1.0), 0.499, material_dielectric_glass),
    Sphere(vec3( 1.0, 0.5, 0.0), 0.499, material_lambert)
);

Box boxes[] = Box[](
    Box(vec3(0,0,0), vec3(0.5,1,0.5), material_dielectric_glass),
    Box(vec3(2.0, 0.0, -3.0), vec3(3.0, 1.0, -2.0), material_box)
);

Plane groundPlane = Plane(
    vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 0.0), material_ground
);

Triangle mirrorTriangle = Triangle(
    vec3(-3.0, 0.0, 0.0), vec3(0.0, 0.0, -4.0), vec3(-1.0, 4.0, -2.0)
);

PointLight pointlights[] = PointLight[](
    // PointLight(vec3( 3.0, 5.0, 3.0), vec3(1.0, 1.0, 1.0), true),
    PointLight(vec3(-3.0, 5.0, 3.0), vec3(0.5, 0.0, 0.0), false),
    PointLight(vec3(-3.0, 5.0,-3.0), vec3(0.0, 0.5, 0.0), false),
    PointLight(vec3( 3.0, 5.0,-3.0), vec3(0.0, 0.0, 0.5), false)
);

TriangleLight arealights[] = TriangleLight[](
    TriangleLight(
        Triangle(
            vec3( 2.0, 5.0, 3.0),
            vec3( 4.0, 5.0, 3.0),
            vec3( 3.0, 5.0, 4.7)
        ),
        vec3(1.0, 1.0, 1.0),
        true
    )
);

// Maps (-inf, inf) to (0, 1).
vec3 visualize(vec3 v)
{
    return 0.5 + tanh(v) * 0.5;
}

// Returns a varying number between 0 and 1.
float rand(vec2 seed)
{
    return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 randUnitDisk(vec3 seed)
{
    float rv0 = rand(seed.xy);
    float r = rv0 * rv0;
    float phi = rand(seed.zx) * 2.0 * PI;
    return r * vec2(cos(phi), sin(phi));
}

vec3 randUnitSphere(vec3 seed)
{
    float rv0 = rand(seed.xy);
    float R = rv0 * rv0 * rv0;
    float theta = acos(1.0 - 2.0 * rand(seed.yz));
    float phi = rand(seed.zx) * 2.0 * PI;
    float cosp = cos(phi);
    float sinp = sin(phi);
    float cost = cos(theta);
    float sint = sin(theta);
    return R * vec3(
        sint * cosp,
        sint * sinp,
        cost
    );
}

// Robert Osada et al., "Shape Distributions", ACM Trans. on Graphics 21(4), 2002.
vec3 randTriangle(vec3 seed, Triangle tri)
{
    float rv0 = rand(seed.xy);
    float rv1 = rand(seed.yz);
    float sqrtrv0 = sqrt(rv0);
    return tri.v0 + sqrtrv0 * (tri.v1 - tri.v0 + rv1 * (tri.v2 - tri.v1));
}

Ray getRay(vec2 uv)
{
    Ray ray;
    ray.origin = cameraPosition;
    vec3 dir = vec3(
        (uv.x - 0.5) * tan(fovY) * W / H,
        (uv.y - 0.5) * tan(fovY),
        -1.0
    );
    ray.direction = normalize(cameraToWorldRotMatrix * dir);
    return ray;
}

bool sphereHit(float t, Sphere sp, Ray r, inout HitRecord hit)
{
    // Already hit by nearer object.
    if (t >= hit.t)
        return false;

    hit.t = t;
    hit.p = r.origin + t * r.direction;
    hit.normal = normalize(hit.p - sp.center);
    hit.mat = sp.mat;
    return true;
}

bool sphereIntersect(Sphere sp, Ray r, inout HitRecord hit)
{
    vec3 co = sp.center - r.origin;
    float tc = dot(co, r.direction);
    vec3 cp = co - tc * r.direction;
    float dist = length(cp);
    // Test if the ray is too far away from the center of the sphere.
    if (dist > sp.radius)
        return false;

    float dt = sqrt(sp.radius * sp.radius - dist * dist);
    float tMin = tc - dt;
    // Hit from the exterior.
    if (tMin >= 0.0)
        return sphereHit(tMin, sp, r, hit);

    float tMax = tc + dt;
    // t_min < 0 and t_max < 0
    if (tMax < 0.0)
        return false;

    // t_min < 0 and t_max >= 0 : Hit from the interior.
    return sphereHit(tMax, sp, r, hit);
}

bool planeIntersect(Plane p, Ray r, inout HitRecord hit)
{
    vec3 n = normalize(p.normal);
    float cosine = dot(r.direction, n);
    // Test if the ray and the plane are parallel.
    if (cosine == 0.0)
        return false;

    float dist = dot(r.origin - p.p0, n);
    // Test if the ray is directed away from the plane.
    if (dist * cosine > 0.0)
        return false;

    float t = -dist / cosine;
    // Already hit by nearer object.
    if (t >= hit.t)
        return false;

    hit.t = t;
    hit.p = r.origin + t * r.direction;
    hit.normal = sign(dist) * n;
    hit.mat = p.mat;
    return true;
}

// Assume an axis-aligned (bounding) box (AABB).
bool boxIntersect(Box b, Ray r, inout HitRecord hit)
{
    vec3 invdir = 1.0 / r.direction;
    vec3 tminTemp = (b.bmin - r.origin) * invdir;
    vec3 tmaxTemp = (b.bmax - r.origin) * invdir;
    bvec3 reversed = lessThan(tmaxTemp, tminTemp);
    bvec3 ordered = not(reversed);
    vec3 reversedF = vec3(reversed);
    vec3 orderedF = vec3(ordered);
    vec3 tmin = tminTemp * orderedF + tmaxTemp * reversedF;
    vec3 tmax = tmaxTemp * orderedF + tminTemp * reversedF;

    float maxtminXY = max(tmin.x, tmin.y);
    float mintmaxXY = min(tmax.x, tmax.y);
    float maxtmin = max(maxtminXY, tmin.z);
    float mintmax = min(mintmaxXY, tmax.z);
    // The straight line do not intersect with the box.
    if (maxtmin > mintmax)
        return false;

    int imaxtmin = 0;
    if (tmin.y > tmin.x)
        imaxtmin = 1;
    if (tmin.z > maxtminXY)
        imaxtmin = 2;

    int imintmax = 0;
    if (tmax.y < tmax.x)
        imintmax = 1;
    if (tmax.z < mintmaxXY)
        imintmax = 2;

    float t;
    int idx;
    // Hit from the exterior.
    if (maxtmin >= 0)
    {
        t = maxtmin;
        idx = imaxtmin;
    }
    // Hit from the interior.
    else if (mintmax >= 0)
    {
        t = mintmax;
        idx = imintmax;
    }
    // The box is behind the ray.
    else
        return false;

    // Ray has already hit by nearer object.
    if (t >= hit.t)
        return false;

    vec3 n = vec3(0.0);
    n[idx] = sign(maxtmin) * sign(-r.direction[idx]);

    hit.t = t;
    hit.p = r.origin + t * r.direction;
    hit.normal = n;
    hit.mat = b.mat;
    return true;
}

bool triangleIntersect(Triangle tri, Ray r, inout HitRecord hit)
{
    // Test if the ray hits the plane containing the triangle.
    vec3 n = normalize(cross(tri.v2 - tri.v0, tri.v1 - tri.v0));
    float cosine = dot(r.direction, n);
    // 1) Test if the ray and the plane are parallel.
    if (cosine == 0.0)
        return false;

    float dist = dot(r.origin - tri.v0, n);
    // 2) Test if the ray is directed away from the plane.
    if (dist * cosine > 0.0)
        return false;

    float t = -dist / cosine;
    // 3) Already hit by nearer object.
    if (t >= hit.t)
        return false;

    // Test if the hitpoint is inside the triangle.
    vec3 p = r.origin + t * r.direction;
    vec3 ep0 = p - tri.v0;
    vec3 ep1 = p - tri.v1;
    vec3 ep2 = p - tri.v2;
    vec3 e10 = tri.v1 - tri.v0;
    vec3 e21 = tri.v2 - tri.v1;
    vec3 e02 = tri.v0 - tri.v2;
    n = sign(dist) * n;
    if (dot(cross(e10, ep0), n) <= 0.0
        || dot(cross(e21, ep1), n) <= 0.0
        || dot(cross(e02, ep2), n) <= 0.0)
        return false;

    hit.t = t;
    hit.p = r.origin + t * r.direction;
    hit.normal = n;

    // Assign temporary material manually for the large triangle.
    hit.mat = material_mirror;

    return true;
}

float schlick(float cosine, float ior)
{
    float r0 = (1.0 - ior) / (1.0 + ior);
    r0 = r0 * r0;

    float cosrev = 1.0 - cosine;
    float cosrev2 = cosrev * cosrev;
    float cosrev4 = cosrev2 * cosrev2;
    float cosrev5 = cosrev4 * cosrev;
    return r0 + (1.0 - r0) * cosrev5;
}

vec3 schlick(float cosine, vec3 r0)
{
    float cosrev = 1.0 - cosine;
    float cosrev2 = cosrev * cosrev;
    float cosrev4 = cosrev2 * cosrev2;
    float cosrev5 = cosrev4 * cosrev;
    return r0 + (vec3(1.0) - r0) * cosrev5;
}

bool trace(Ray r, out HitRecord hit)
{
    // Trace a single ray.
    hit.t = 1.0 / 0.0; // Infinity
    hit.p = r.origin;
    hit.normal = r.direction;

    bool hitAny = false;
    bool hitThis;

    // Sphere
    for (int i = 0; i < NUM_SPHERES; ++i)
    {
        hitThis = sphereIntersect(spheres[i], r, hit);
        hitAny = hitAny || hitThis;
    }

    // Box
    for (int i = 0; i < NUM_BOXES; ++i)
    {
        hitThis = boxIntersect(boxes[i], r, hit);
        hitAny = hitAny || hitThis;
    }

    // Plane
    hitThis = planeIntersect(groundPlane, r, hit);
    hitAny = hitAny || hitThis;
    
    // Triangle
    hitThis = triangleIntersect(mirrorTriangle, r, hit);
    hitAny = hitAny || hitThis;
    
    return hitAny;
}

vec3 calculateShadow(HitRecord hit, vec3 lightDir)
{
    Ray shadowRay;
    HitRecord shadowHit;
    vec3 shadowAttn = vec3(0.0);
    for (int s = 0; s < NUM_SAMPLES_SHADOW; ++s)
    {
        // Generate a shadow ray.
        shadowRay = Ray(
            hit.p + hit.normal * EPSILON,
            normalize(lightDir + SHADOW_JITTER * randUnitSphere(hit.p + s))
        );
        
        // First hit.
        vec3 shadowAttnSample = vec3(1.0);
        if (trace(shadowRay, shadowHit))
        {
            // Dielectric material casts a semi-transparent shadow.
            if (shadowHit.mat.scatter_type == SCATTER_TYPE_REFRACTIVE)
            {
                // Ray goes into and then out of a dielectric material.
                // Due to the performance issue, we check only two
                // dielectric mateirals blocking the way.
                shadowRay = Ray(
                    shadowHit.p - shadowHit.normal * EPSILON,
                    lightDir
                );
                shadowAttnSample *= (vec3(1.0) - schlick(
                    abs(dot(shadowRay.direction, shadowHit.normal)),
                    vec3(1.0) - shadowHit.mat.shadow_attenuation_constant
                ));
                // Ignore the next hit since the ray would hit the same
                // material twice. Note that this does not hold when objects
                // are collided.
                trace(shadowRay, shadowHit);

                // Second hit.
                shadowRay = Ray(
                    shadowHit.p + shadowHit.normal * EPSILON,
                    lightDir
                );
                if (trace(shadowRay, shadowHit))
                {
                    // Check if the second hit object has dielectric material.
                    if (shadowHit.mat.scatter_type == SCATTER_TYPE_REFRACTIVE)
                    {
                        shadowRay = Ray(
                            shadowHit.p - shadowHit.normal * EPSILON,
                            lightDir
                        );
                        shadowAttnSample *= (vec3(1.0) - schlick(
                            abs(dot(shadowRay.direction, shadowHit.normal)),
                            vec3(1.0) - shadowHit.mat.shadow_attenuation_constant
                        ));
                        // Ignore the next hit
                        trace(shadowRay, shadowHit);

                        // Third hit.
                        shadowRay = Ray(
                            shadowHit.p + shadowHit.normal * EPSILON,
                            lightDir
                        );
                        if (trace(shadowRay, shadowHit))
                            shadowAttnSample = vec3(0.0);
                    }
                    // Second hit but not a dielectric.
                    else
                        shadowAttnSample = vec3(0.0);
                }
            }
            // First hit but not a dielectric.
            else
                shadowAttnSample = vec3(0.0);
        }
        shadowAttn += shadowAttnSample;
    }
    shadowAttn /= NUM_SAMPLES_SHADOW;
    return shadowAttn;
}

vec3 calculateDiffuseSpecular(HitRecord hit, vec3 lightDir, vec3 lightColor, bool castShadow)
{
    vec3 shadowAttn = vec3(0.0);
    if (castShadow)
        shadowAttn = calculateShadow(hit, lightDir);

    // 2. Diffuse
    float diffuseCosine = max(dot(hit.normal, lightDir), 0.0);
    vec3 diffuse = diffuseCosine * hit.mat.Kd;

    // 3. Specular
    vec3 viewDir = normalize(cameraPosition - hit.p);
    vec3 reflectDir = reflect(-lightDir, hit.normal);
    float specularCosine = max(dot(viewDir, reflectDir), 0.0);
    float specularFactor = pow(specularCosine, hit.mat.shininess);
    vec3 specular = specularCosine * hit.mat.Ks;

    // Phong lighting for each light sources.
    return shadowAttn * (specular + diffuse) * lightColor;
}

vec3 phongIllumination(HitRecord hit, Ray ray)
{
    // Do Phong lighting.
    // 1. Ambient
    vec3 ambient = hit.mat.Ka;
    vec3 phong = ambient * ambientLightColor;

    // Diffuse and specular lighting for each point light source.
    for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
    {
        // Calculate shadow with additional ray casting.
        vec3 lightDir = normalize(pointlights[i].position - hit.p);

        // Phong lighting for each light sources.
        phong += calculateDiffuseSpecular(
            hit, lightDir, pointlights[i].color, pointlights[i].castShadow
        );
    }

    // Diffuse and specular lighting for each area light source.
    // Assume all area lights to be triangular for brevity.
    for (int i = 0; i < NUM_AREA_LIGHTS; ++i)
    {
        // Calculate shadow with additional ray casting.
        // Light direction of an area light should be arbitraty.
        vec3 lightPos = randTriangle(hit.p + i, arealights[i].geom);
        vec3 lightDir = normalize(lightPos - hit.p);

        // Phong lighting for each light sources.
        phong += calculateDiffuseSpecular(
            hit, lightDir, arealights[i].color, arealights[i].castShadow
        );
    }
    return clamp(phong, 0.0, 1.0);
}

bool mirrorScatter(HitRecord hit, inout Ray ray, inout vec3 attenuation)
{
    float cosine = dot(ray.direction, hit.normal);
    vec3 rayBiasedOrigin = hit.p + -sign(cosine) * hit.normal * EPSILON;
    vec3 reflection = reflect(ray.direction, hit.normal);
    ray = Ray(rayBiasedOrigin, normalize(reflection));
    attenuation *= schlick(abs(cosine), hit.mat.R0);
    return true;
}

bool lambertianScatter(HitRecord hit, inout Ray ray, inout vec3 attenuation)
{
    float cosine = dot(ray.direction, hit.normal);
    vec3 rayBiasedOrigin = hit.p + -sign(cosine) * hit.normal * EPSILON;
    ray = Ray(rayBiasedOrigin, normalize(hit.normal + randUnitSphere(hit.p)));
    attenuation *= hit.mat.R0;
    return true;
}

bool refractBool(vec3 v, vec3 n, float eta, out vec3 refracted)
{
    float cosi = dot(v, n);
    float costsq = 1.0 - eta * eta * (1 - cosi * cosi);
    if (costsq > 0)
    {
        refracted = eta * (v - n * cosi) - n * sqrt(costsq);
        return true;
    }
    else
        return false;
}

bool refractiveScatter(HitRecord hit, inout Ray ray, inout vec3 attenuation)
{
    // Calculate the refraction/reflection ratio and determine the next ray.
    float eta = 1.0 / hit.mat.ior; // ni/nt
    float cosine = -dot(ray.direction, hit.normal);
    float dir = sign(cosine);
    // Ray is inside the material.
    if (dir < 0.0)
    {
        eta = hit.mat.ior;
        cosine *= -eta;

        // Apply Beer-Lambert law to importance sample the next ray.
        // vec3 rv0 = vec3(
        //     rand(ray.direction.xx + hit.p.xy),
        //     rand(ray.direction.yy + hit.p.yz),
        //     rand(ray.direction.zz + hit.p.zx)
        // );
        vec3 kappa = hit.mat.extinction_constant;
        vec3 transmittance = kappa * exp(-kappa * hit.t);
        attenuation *= transmittance;
        // attenuation *= vec3(lessThan(rv0, transmittance));
    }

    // Reflect or refract according to the reflection ratio.
    float rv1 = rand(ray.direction.xy + hit.p.zy);

    float reflectRatio = float(1.0);
    vec3 refraction = vec3(0.0);
    if (refractBool(ray.direction, dir * hit.normal, eta, refraction))
        reflectRatio = schlick(cosine, eta);

    vec3 reflection = reflect(ray.direction, hit.normal);
    vec3 rayBiasedOrigin = hit.p - dir * hit.normal + EPSILON;
    if (rv1 < reflectRatio)
        ray = Ray(rayBiasedOrigin, normalize(reflection));
    else
        ray = Ray(rayBiasedOrigin, normalize(refraction));
    return true;
}

bool specularScatter(HitRecord hit, inout Ray ray, inout vec3 attenuation)
{
    float cosine = dot(ray.direction, hit.normal);
    vec3 rayBiasedOrigin = hit.p + -sign(cosine) * hit.normal * EPSILON;
    vec3 reflection = reflect(ray.direction, hit.normal);
    ray = Ray(
        rayBiasedOrigin,
        normalize(reflection + 0.0001 * randUnitSphere(hit.p))
    );
    attenuation *= schlick(abs(cosine), hit.mat.R0);
    return dot(ray.direction, hit.normal) > 0.0;
}

vec3 castRay(Ray r)
{
    // Trace a ray in iterative way.
    Ray currentRay = r;
    HitRecord hit;
    
    // Return the default (miss) color.
    vec3 attenuation = vec3(1.0);
    vec3 color = vec3(0.0);
    bool flagStopIteration = false;
    for (int b = 0; b < MAX_DEPTH; ++b)
    {
        if (!trace(currentRay, hit))
        {
            color += attenuation
                * texture(environmentMap, currentRay.direction).rgb;
            break;
        }

        vec3 deltaColor = attenuation * phongIllumination(hit, currentRay);
        switch (hit.mat.scatter_type)
        {
        case SCATTER_TYPE_PHONG:
            color += deltaColor;
            flagStopIteration = !mirrorScatter(hit, currentRay, attenuation);
            break;
        case SCATTER_TYPE_LAMBERTIAN:
            color += deltaColor;
            flagStopIteration = !lambertianScatter(hit, currentRay, attenuation);
            break;
        case SCATTER_TYPE_REFRACTIVE:
            flagStopIteration = !refractiveScatter(hit, currentRay, attenuation);
            break;
        case SCATTER_TYPE_SPECULAR:
            color += deltaColor;
            flagStopIteration = !specularScatter(hit, currentRay, attenuation);
            break;
        default:
            // No illumination.
            flagStopIteration = true;
            break;
        }

        // Next iteration.
        if (flagStopIteration)
            break;
    }

    return color;
}

void main()
{
    vec3 color = vec3(0.0);
    Ray r;
    for (int s = 0; s < NUM_SAMPLES; ++s)
    {
        vec2 coord = TexCoord + 0.001 * randUnitDisk(color + s);
        r = getRay(coord);
        color += castRay(r);
    }
    color /= NUM_SAMPLES;
    FragColor = vec4(color, 1.0);
}
