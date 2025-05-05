#version 410 core

// shaderf_lightingpasspbr.fs
// Physically-based lighting pass fragment shader for deferred shading — workhorse
// Space: Viewspace

// OUTPUTS
layout (location = 0) out vec4 FragColor;

// INPUTS
in vec2 TexCoords;

// UNIFORMS
layout (std140) uniform Matrices_Fragment {
    mat4 inv_view;
};
layout (std140) uniform Camera {  // this is 2 * sizeof(glm::mat4)
    vec3 cam_position;
    vec3 cam_front;
};
struct DirLight_t {
    vec4 color;
    vec4 direction; // dir_view.x, dir_view.y, dir_view.z, shadowmap_index
    mat4 matrix;    
};
layout (std140) uniform DirLight {
    float dirlight_count;
    DirLight_t dirlights[8];
};
struct PointLight_t {
    vec4 color;
    vec4 attenuation;       // constant, linear, quadratic, radius
    vec4 position;          // pos.x, pos.y, pos.z, shadowmap_cube_farplane
    vec4 position_world;    // pos_world.x, pos_world.y, pos_world.z, shadowmap_index
};
layout (std140) uniform PointLight {
    float pointlight_count;
    PointLight_t pointlights[32];
};

// G-BUFFER
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gMetRouOcc;

// SHADOW MAPPING
uniform sampler2DArrayShadow shadowmap_2d_array_shadow;
uniform samplerCubeArrayShadow shadowmap_cube_array_shadow;

// SSAO
uniform bool ssao = false;
uniform sampler2D ssaoMap;

// PBR
uniform float ibl = 0.0;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// FORWARD DECLARATIONS
vec3 CalcDirLightPBR(vec4 pos, vec3 normal, vec3 albedo, float metallic, float roughness, vec3 viewDir, float occlusion);
vec3 CalcPointLightPBR(vec4 pos, vec3 normal, vec3 albedo, float metallic, float roughness, vec3 viewDir, float occlusion);
vec3 CalcIBL(vec3 normal, vec3 albedo, float metallic, float roughness, vec3 viewdir, float occlusion);
float CalcShadow2D(int lightIndex, vec3 normal, vec4 fragPos);
float CalcShadowCube(int lightIndex, vec4 fragPos);

float DistributionGGX(vec3 N, vec3 H, float roughness);
vec3 FresnelSchlick(vec3 H, vec3 V, vec3 F0);
float _GeometrySchlickGGX(vec3 N, vec3 V, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 W, float roughness);

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

// const float AMBIENT_FACTOR = 1e-3;
const float PI = 3.14159265359;

// ============== MAIN ==============
void main() {
    // Process G-buffer
    vec4 FragPos = texture(gPosition, TexCoords);
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    vec4 MetRouOcc = texture(gMetRouOcc, TexCoords);
    float Metallic = MetRouOcc.r;
    float Roughness = MetRouOcc.g;
    float Occlusion = (MetRouOcc.b == 1 || !ssao) ? MetRouOcc.a : texture(ssaoMap, TexCoords).r;

    // Process other inputs
    vec3 viewDir = vec3(normalize(-FragPos));

    // Compute light contributions
    vec3 result = vec3(0);
    result += CalcDirLightPBR(FragPos, Normal, Albedo, Metallic, Roughness, viewDir, Occlusion);
    result += CalcPointLightPBR(FragPos, Normal, Albedo, Metallic, Roughness, viewDir, Occlusion);
    if (ibl > 0.0)
        result += ibl * CalcIBL(Normal, Albedo, Metallic, Roughness, viewDir, Occlusion);

    // Final output
    FragColor = vec4(result, 1.0);
}
// ==================================

// FUNCTION DEFINITIONS
vec3 CalcDirLightPBR(vec4 pos, vec3 normal, vec3 albedo, float metallic, float roughness, vec3 viewdir, float occlusion) {
    vec3 Lo = vec3(0);
    for (int i = 0; i < dirlight_count; i++) {
        // Light and halfway directions
        vec3 Wi = normalize(-dirlights[i].direction.xyz); // Point-to-light
        vec3 H = normalize(viewdir + Wi); // Halfway

        // Incoming spectrial radiance from dirlight
        vec3 Li = dirlights[i].color.rgb;   // Radiance

        // Reflected spectral radiance: Cook-Torrance BRDF
        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, metallic);
        vec3 F = FresnelSchlick(H, viewdir, F0);
        float NDF = DistributionGGX(normal, H, roughness);
        float G = GeometrySmith(normal, viewdir, Wi, roughness);

        vec3 num = NDF * G * F;
        float denom = 4.0 * max(dot(Wi, normal), 0.0) * max(dot(viewdir, normal), 0.0) + 1e-4;
        vec3 specular = num / denom;

        // Emitted spectral radiance: Lambertian diffuse
        vec3 kS = F;
        vec3 kD = vec3(1) - kS;
        kD *= 1.0 - metallic;
        vec3 diffuse = kD * albedo / PI;

        // Shadows
        float shadowFactor = 1.0;
        if (dirlights[i].direction.w >= 0)
            shadowFactor -= CalcShadow2D(i, normal, pos);

        Lo += shadowFactor * ((diffuse) + (specular)) * Li * max(dot(normal, Wi), 0.0);
        // vec3 ambient = AMBIENT_FACTOR * albedo * occlusion;
    }
    
    return Lo;
}

vec3 CalcPointLightPBR(vec4 pos, vec3 normal, vec3 albedo, float metallic, float roughness, vec3 viewdir, float occlusion) {
    vec3 Lo = vec3(0);
    // Sum over all point lights
    for (int i = 0; i < pointlight_count; i++) {
        // Light and halfway directions
        vec3 Wi = normalize(pointlights[i].position.xyz - pos.xyz); // Point-to-light_i
        vec3 H = normalize(viewdir + Wi); // Halfway

        // Incoming spectrial radiance from pointlight i
        float dist = distance(pointlights[i].position.xyz, pos.xyz);
        float attenuation = 1.0 / ( pointlights[i].attenuation.x + pointlights[i].attenuation.y*dist + pointlights[i].attenuation.z*dist*dist );
        vec3 Li = pointlights[i].color.rgb * attenuation;   // Radiance

        // Reflected spectral radiance: Cook-Torrance BRDF
        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, metallic);
        vec3 F = FresnelSchlick(H, viewdir, F0);
        float NDF = DistributionGGX(normal, H, roughness);
        float G = GeometrySmith(normal, viewdir, Wi, roughness);

        vec3 num = NDF * G * F;
        float denom = 4.0 * max(dot(Wi, normal), 0.0) * max(dot(viewdir, normal), 0.0) + 1e-4;
        vec3 specular = num / denom;

        // Emitted spectral radiance: Lambertian diffuse
        vec3 kS = F;
        vec3 kD = vec3(1) - kS;
        kD *= 1.0 - metallic;
        vec3 diffuse = kD * albedo / PI;

        // Shadows
        float shadowFactor = 1.0;
        if (pointlights[i].position_world.w >= 0)
            shadowFactor -= CalcShadowCube(i, pos);

        Lo += shadowFactor * ((diffuse) + (specular)) * Li * max(dot(normal, Wi), 0.0);
    }    
    // vec3 ambient = AMBIENT_FACTOR * albedo * occlusion;
    return Lo;
}

float CalcShadow2D(int lightIndex, vec3 normal, vec4 fragPos) {
    vec4 fragPosLightSpace = dirlights[lightIndex].matrix * inv_view * fragPos;  // can be vec4(fragPos.xyz,1)
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;    // Map from [-1,1] to [0,1]
    float currentDepth = projCoords.z;
    
    // Shadow map antialiasing
    float offset = 2;
    float stepsize = 0.5;
    
    float shadow = 0.0;
    vec2 texelSize = vec2(1.0 / textureSize(shadowmap_2d_array_shadow, 0));
    int shadowIndex = int(dirlights[lightIndex].direction.w);
    float bias = max(0.003*(1.0 - dot(normal, dirlights[lightIndex].direction.xyz)), 0.005);

    for (float x = -offset; x <= offset; x += stepsize) {
        for (float y = -offset; y <= offset; y += stepsize) {
            float result = texture(shadowmap_2d_array_shadow, vec4(projCoords.xy + vec2(x,y) * texelSize, shadowIndex, currentDepth-bias));
            shadow += 1.0 - result;
        }
    }
    return shadow/(2*offset/stepsize+1)/(2*offset/stepsize+1);
}

float CalcShadowCube(int lightIndex, vec4 fragPos) {
    vec3 fragToLight = vec3(inv_view * fragPos) - pointlights[lightIndex].position_world.xyz;
    float currentDepth = length(fragToLight);
    float viewDistance = length(vec3(fragPos));
    
    // Shadow map antialiasing
    float bias = 0.005;

    float shadow = 0.0;
    float farPlane = pointlights[lightIndex].position.w;
    float diskRadius = 0.01 + 0.05 * viewDistance/farPlane; // Sharper shadows when closer, softer shadows when farther
    int shadowIndex = int(pointlights[lightIndex].position_world.w);
    for (int i = 0; i < 20; i++) {
        float result = texture(shadowmap_cube_array_shadow, vec4(fragToLight + sampleOffsetDirections[i] * diskRadius, shadowIndex), currentDepth/farPlane - bias);
        shadow += 1.0 - result;
    }

    return shadow/20.0;
}

// Normal distribution function: Trowbridge-Reitz GGX
// Description: Estimates microfacets exactly aligned with halfway vector
// - N: surface normal vector
// - H: halfway vector
// - roughness: roughness of surface
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a * a;
    
    float NdotH = max(dot(N,H), 0.0);
    float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
    denom = PI * denom * denom;

    return a2 / denom;
}

// Geometry function: Schlick-GGX
// Description: Approximates geometry shadowing and geometry obstruction due to microfacets
// - N: surface normal vector
// - V: camera-to-frag vector
// - W: frag-to-light vector
// - roughness: roughness of surface
float _GeometrySchlickGGX(vec3 N, vec3 V, float roughness) {
    float a = roughness + 1.0;
    float k = a*a / 8.0;    // k_direct

    float NdotV = max(dot(N,V), 0.0);
    return NdotV / (NdotV * (1-k) + k);
}
float GeometrySmith(vec3 N, vec3 V, vec3 W, float roughness) {
    float ggx1 = _GeometrySchlickGGX(N, V, roughness);
    float ggx2 = _GeometrySchlickGGX(N, W, roughness);
    return ggx1 * ggx2;
}

// Fresnel equation
// Description: Ratio of light that gets reflected
// - H: halfway vector
// - V: camera-to-frag vector
// - F0: surface reflection at zero incidence
vec3 FresnelSchlick(vec3 H, vec3 V, vec3 F0) {
    float cosTheta = max(dot(H, V), 0.0);
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5);
}
vec3 FresnelSchlickRoughness(vec3 H, vec3 V, vec3 F0, float roughness) {
    float cosTheta = max(dot(H, V), 0.0);
    return F0 + (max(vec3(1.0-roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5);
}

vec3 CalcIBL(vec3 normal, vec3 albedo, float metallic, float roughness, vec3 viewdir, float occlusion) {
    float NdotV = max(dot(normal,viewdir), 0.0);
    vec3 reflection = reflect(-viewdir, normal);
    vec3 reflection_world = mat3(inv_view) * reflection;
    vec3 normal_world = mat3(inv_view) * normal;

    vec3 F0 = vec3(0.04);
    vec3 F = FresnelSchlickRoughness(normal, viewdir, F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    // Indirect diffuse irradiation
    vec3 irradiance = texture(irradianceMap, normal_world).rgb;
    vec3 diffuse = irradiance * albedo;
    
    // Indirect specular irradiation
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, reflection_world, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 envBRDF = texture(brdfLUT, vec2(NdotV, roughness)).rg;
        // Split sum approximation: Prefilter * BRDF
    vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

    vec3 ambient = (kD*diffuse + specular) * occlusion;

    return ambient;
}