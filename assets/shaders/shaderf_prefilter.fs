#version 410 core

in vec3 localPos;
out vec4 FragColor;

uniform samplerCube envCubemap;
uniform float roughness;
uniform int envResolution;

const float PI = 3.14159265359;

float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint n);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);

void main() {
    vec3 N = normalize(localPos);  // Based on view matrix supplied in vertex shader
    vec3 R = N; // Reflection direction
    vec3 V = R; // View direction

    const uint SAMPLE_COUNT = 1024u;
    float totalWeight = 0.0;
    vec3 prefilteredColor = vec3(0);
    for (uint i = 0u; i < SAMPLE_COUNT; i++) {
        // Generate texture sample vector
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);          // Low-disrepancy sample with bounds <[0,1], [0,1)>
        vec3 H = ImportanceSampleGGX(Xi, N, roughness); // GGXTR NDF-sampled halfway vector
        // vec3 L = normalize(2.0 * dot(V,H) * H - V);     // Frag-to-light vector (i.e., cubemap sample vector). Reflection calculation.
        vec3 L = -normalize(reflect(V, H));             
        float NdotL = max(dot(N,L),0.0);   // Sample weight = alignment of L with N (i.e., closer to normal has greater weight)

        // Sample environment cubemap and apply weight
        if (NdotL > 0.0) {
            // Determine mip level
            float D = DistributionGGX(N, H, roughness);
            float pdf = (D * max(dot(N,H),0.0) / (4.0 * max(dot(H,V),0.0))) + 0.0001;
            float saTexel = 4.0 * PI / (6.0 * envResolution * envResolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);
            float mipLevel = (roughness == 0.0) ? 0.0 : 0.5 * log2(saSample/saTexel);

            prefilteredColor += textureLod(envCubemap, L, mipLevel).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    prefilteredColor /= totalWeight;

    FragColor = vec4(prefilteredColor, 1);
}

// Mirrors an integer around its decimal point
// - bits: unsigned integer to reverse bits of
float RadicalInverse_VdC(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// Generates a sample belonging to a low-discrepancy sample sequence?
// - i: sample index
// - n: total number of samples
vec2 Hammersley(uint i, uint n) {
    // x: relative sample index [0,1]           e.g., 921/1024=0.891
    // y: decimal-inversed sample index [0,1)   e.g., 0.129
    return vec2( float(i)/float(n), RadicalInverse_VdC(i) );
}

// Generates a sample vector oriented around a microsurface's halfway vector, given its normal.
// - Xi: low-disrepancy sequence value
// - normal: surface normal
// - roughness: surface roughness
vec3 ImportanceSampleGGX(vec2 Xi, vec3 normal, float roughness) {
    float a = roughness*roughness;
    float a2 = a * a;

    // Tangent space
        // Phi - based linearly on sample index
    float phi = 2.0 * PI * Xi.x;
        // Theta - based on low-discrepancy sample of GGXTR NDF
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a2 - 1.0) * Xi.y));    // GGXTR NDF, solving for (n.h) (or something like this)
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
    vec3 H = vec3( cos(phi)*sinTheta, sin(phi)*sinTheta, cosTheta );

    // Change basis to world space
    vec3 up = abs(normal.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
    vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tangentToWorld = mat3(tangent, bitangent, normal);
    // vec3 sampleVec = tangentToWorld * H;
    vec3 sampleVec = tangent * H.x + bitangent * H.y + normal * H.z;

    return normalize(sampleVec);
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