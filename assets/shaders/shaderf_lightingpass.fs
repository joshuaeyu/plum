#version 410 core

// shaderf_lightingpass.fs
// Lighting pass fragment shader for deferred shading — workhorse
// Space: Viewspace

// OUTPUTS
layout (location = 0) out vec4 FragColor;   // GL_COLOR_ATTACHMENT0 from glDrawBuffers
layout (location = 1) out vec4 BrightColor; // GL_COLOR_ATTACHMENT1 from glDrawBuffers

// INPUTS
in vec2 TexCoords;

// UNIFORMS
layout (std140) uniform Matrices_Fragment {
    mat4 inv_view;
};
layout (std140) uniform Camera {  // this is 2 * sizeof(glm::mat4)
    vec3 position;
    vec3 front;
};
layout (std140) uniform DirLight {
    vec3 dirlight_color;
    vec3 dirlight_direction;
    mat4 dirlight_matrix;
};
struct PointLight_t {
    vec4 color;
    vec4 attenuation;       // constant, linear, quadratic, radius
    vec4 position;
    vec4 position_world;    // pos_world.x, pos_world.y, pos_world.z, has_shadows
};
layout (std140) uniform PointLight {
    float pointlight_count;
    PointLight_t pointlights[32];
};

// G-BUFFER
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

// SHADOW MAPPING
uniform sampler2D shadowmap;
uniform samplerCube shadowmap_cube;
uniform samplerCubeArray shadowmap_cube_array;
// uniform samplerCubeArrayShadow shadowmap_cube_array_shadow;
uniform float shadowmap_cube_far;

// SSAO
uniform sampler2D ssao;

// FORWARD DECLARATIONS
vec3 CalcDirLight(vec3 normal, vec3 viewDir, vec3 albedo, float occlusion, float specularIntensity, vec4 fragPos);
vec3 CalcPointLight(vec3 normal, vec3 viewDir, int i, vec3 albedo, float occlusion, float specularIntensity, vec4 fragPos);
float CalcShadow(vec3 normal, vec3 lightDir, vec4 fragPos);
float CalcShadowCube(int index, vec4 fragPos);

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

const float AMBIENT_FACTOR = 1e-3;

// ============== MAIN ==============
void main() {
    // Process G-buffer
    vec4 FragPos = texture(gPosition, TexCoords);
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    float AmbientOcclusion = texture(ssao, TexCoords).r;

    // Process other inputs
    vec3 viewDir = vec3(normalize(-FragPos));

    // Compute light contributions
    vec3 result = vec3(0);
    result += CalcDirLight(Normal, viewDir, Albedo, AmbientOcclusion, Specular, FragPos);
    for (int i = 0; i < pointlight_count; i++) {
        result += CalcPointLight(Normal, viewDir, i, Albedo, AmbientOcclusion, Specular, FragPos);
    }

    // // Output to two colorbuffers
    // FragColor = vec4(result, 1.0);
    FragColor = vec4(result, 1.0);
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0, 0, 0, 1.0);

    // // Debugging
    // float closestDepth = texture(shadowmap_cube, vec3(inv_view * FragPos) - pointlight_pos_world[0]).r;
    // FragColor = vec4(vec3(closestDepth), 1);
    // float currentDepth = length(vec3(inv_view * FragPos) - pointlight_pos_world[0]);
    // FragColor = vec4(vec3(currentDepth/50),1);
    // float currentDepth = length(FragPos.xyz - pointlight_pos[0]);
    // FragColor = vec4(vec3(inv_view * FragPos),1);
}
// ==================================

// FUNCTION DEFINITIONS
vec3 CalcDirLight(vec3 normal, vec3 viewDir, vec3 albedo, float occlusion, float specularIntensity, vec4 fragPos) {
    vec3 lightDir = normalize(-dirlight_direction); // viewspace
    
    // --- Calculate coefficients ---
    // Diffuse
    float diff = max( dot(normal,lightDir), 0.0 );
    // Specular - Blinn-Phong
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow( max( dot(normal,halfwayDir), 0.0), 16 );

    // --- Calculate components ---
    vec3 ambient = albedo * occlusion * AMBIENT_FACTOR;
    vec3 diffuse = albedo * dirlight_color * diff;
    vec3 specular = albedo * specularIntensity * dirlight_color * spec;

    // --- Shadow ---
    float shadowFactor = 1.0 - CalcShadow(normal, lightDir, fragPos);

    return (ambient + shadowFactor*(diffuse + specular));
}

vec3 CalcPointLight(vec3 normal, vec3 viewDir, int i, vec3 albedo, float occlusion, float specularIntensity, vec4 fragPos) {
    vec3 lightDir = normalize(pointlights[i].position.xyz-fragPos.xyz); // viewspace
    
    // --- Calculate coefficients ---
    // Diffuse
    float diff = max( dot(normal,lightDir), 0.0 );
    // Specular - Blinn-Phong
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow( max( dot(normal,halfwayDir), 0.0), 16 );

    // --- Calculate components ---
    vec3 ambient = albedo * occlusion * AMBIENT_FACTOR;
    vec3 diffuse = albedo * pointlights[i].color.xyz * diff;
    vec3 specular = albedo * specularIntensity * pointlights[i].color.xyz * spec;

    // --- Attenuation ---
    float dist = distance(pointlights[i].position.xyz, fragPos.xyz);
    float attenuation = 1.0 / ( pointlights[i].attenuation.x + pointlights[i].attenuation.y*dist + pointlights[i].attenuation.z*dist*dist );
    // float attenuation = 1.0 / ( 1.0 + 0.045*dist + 0.0075*dist*dist );

    // --- Shadow ---
    float shadowFactor = 1.0;
    if (pointlights[i].position_world.w == 1)
        shadowFactor -= CalcShadowCube(i, fragPos);

    return (ambient + shadowFactor*(diffuse + specular)) * attenuation;
}

float CalcShadow(vec3 normal, vec3 lightDir, vec4 fragPos) {
    vec4 fragPosLightSpace = dirlight_matrix * inv_view * fragPos;  // can be vec4(fragPos.xyz,1)
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;    // Map from [-1,1] to [0,1]
    
    // Shadow map antialiasing
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowmap, 0);
    float offset = 3.5;
    for (float x = -offset; x <= offset; x++) {
        for (float y = -offset; y <= offset; y++) {
            float closestDepth = texture(shadowmap, projCoords.xy + vec2(x,y) * texelSize).r;
            float currentDepth = projCoords.z;
            float bias = max(0.005*(1.0 - dot(normal, lightDir)), 0.005);
            // float bias = 0.05;
            if (projCoords.z > 1.0)
                shadow += 0.0;
            else if (currentDepth - bias > closestDepth)
                shadow += 1.0;
        }
    }
    return shadow/(2*offset+1)/(2*offset+1);
}

float CalcShadowCube(int index, vec4 fragPos) {
    vec3 fragToLight = vec3(inv_view * fragPos) - pointlights[index].position_world.xyz;  // Cubemap sample direction needs to be in worldspace. Unless we can somehow generate the cubemap in camera's viewspace.
    float currentDepth = length(fragToLight);
    float viewDistance = length(vec3(fragPos));
    
    float shadow = 0.0;
    float bias = 0.005;
    float diskRadius = 0.01 + 0.05 * viewDistance/shadowmap_cube_far; // Sharper shadows when closer, softer shadows when farther

    for (int i = 0; i < 20; i++) {
        // float closestDepth = texture(shadowmap_cube, fragToLight + sampleOffsetDirections[i] * diskRadius).r;   
        float closestDepth = texture(shadowmap_cube_array, vec4(fragToLight + sampleOffsetDirections[i] * diskRadius, index)).r;   
        // float closestDepth = texture(shadowmap_cube_array_shadow, vec4(fragToLight + sampleOffsetDirections[i] * diskRadius, index)).r;   
        closestDepth *= shadowmap_cube_far;
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    return shadow/20.0;
}