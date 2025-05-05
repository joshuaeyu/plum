#version 410 core

in vec2 TexCoords;
out vec2 FragColor;

const float PI = 3.14159265359;

float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint n);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness);
vec2 IntegrateBRDF(float NdotV, float roughness); 

void main() {
    FragColor = IntegrateBRDF(TexCoords.x, TexCoords.y);
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
    vec3 sampleVec = tangentToWorld * H;

    return normalize(sampleVec);
}

// Geometry function: Schlick-GGX
// Description: Approximates geometry shadowing and geometry obstruction due to microfacets
// - N: surface normal vector
// - V: camera-to-frag vector
// - W: frag-to-light vector
// - roughness: roughness of surface
float _GeometrySchlickGGX(vec3 N, vec3 V, float roughness) {
    float a = roughness;
    float k = a*a / 2.0;    // k_IBL

    float NdotV = max(dot(N,V), 0.0);
    return NdotV / (NdotV * (1-k) + k);
}
float GeometrySmith(vec3 N, vec3 V, vec3 W, float roughness) {
    float ggx1 = _GeometrySchlickGGX(N, V, roughness);
    float ggx2 = _GeometrySchlickGGX(N, W, roughness);
    return ggx1 * ggx2;
}

// first input is n . w_i
vec2 IntegrateBRDF(float NdotV, float roughness) {
    // DFG/4wo*n wi*n
    vec3 V = vec3( sqrt(1.0-NdotV*NdotV), 0.0, NdotV);
    vec3 N = vec3(0,0,1);

    float A = 0.0;
    float B = 0.0;

    const uint SAMPLE_COUNT = 1024u;
    for (uint i = 0u; i < SAMPLE_COUNT; i++) {
        // Generate texture sample vector
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);          // Low-disrepancy sample with bounds <[0,1], [0,1)>
        vec3 H = ImportanceSampleGGX(Xi, N, roughness); // GGXTR NDF-sampled halfway vector
        // vec3 L = normalize(2.0 * dot(V,H) * H - V);     // Frag-to-light vector (i.e., cubemap sample vector). Reflection calculation.
        vec3 L = -normalize(reflect(V, H));             
        float NdotL = max(L.z, 0.0);   // Sample weight = alignment of L with N (i.e., closer to normal has greater weight). This works because N is always <0,0,1>.

        // Compute BRDF
        if (NdotL > 0.0) {
            float NdotH = max(H.z, 0.0);
            float VdotH = max(dot(V, H), 0.0);
            float G = GeometrySmith(N, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);
            
            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    return vec2(A,B) / float(SAMPLE_COUNT);
}