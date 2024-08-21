#version 410 core

in vec2 TexCoords;
out float FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noiseTexture; // 16 random vectors on the TB plane

uniform vec3 samples[64];   // 64 random vectors in the N-facing hemisphere
uniform mat4 projection;

const vec2 noiseScale = vec2(1920./4., 1080./4.);

const int KERNEL_SIZE = 64;
const float RADIUS = 0.5;

void main() {
    // Process G-buffer
    vec4 FragPos = texture(gPosition, TexCoords);
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 RandomVec = texture(noiseTexture, TexCoords * noiseScale).xyz;
    
    // Compute TBN matrix
    vec3 T = normalize(RandomVec - dot(RandomVec, Normal) * Normal);    // T = component of RandomVec that is orthogonal to Normal
    vec3 B = cross(Normal, T);
    mat3 TBN = mat3(T, B, Normal);

    // Sample from kernel
    float occlusion = 0;
    for (int i = 0; i < KERNEL_SIZE; i++) {
        // Viewspace sample vector
        // - sample some position in the normal-facing hemisphere of the current fragment
        vec3 samplePos = TBN * samples[i];
        samplePos = FragPos.xyz + samplePos * RADIUS;
        // Clip space position of sample
        vec4 offset = projection * vec4(samplePos, 1.0);
        offset.xyz /= offset.w; // Perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // Map to [0,1]
        // Occlude fragment if sample is behind its corresponding G-buffer point
        // - compare z-coordinates, not depth values!
        float actualZ = texture(gPosition, offset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, RADIUS / abs(FragPos.z - actualZ)); // Only apply occlusion if fragment and G-buffer point sampled are within RADIUS of each other 
        float bias = 0.001;
        if (actualZ >= samplePos.z + bias)
            occlusion += 1.0 * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / KERNEL_SIZE);
    FragColor = occlusion;
}