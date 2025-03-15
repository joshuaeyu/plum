#version 410 core

in vec3 localPos;
out vec4 FragColor;

uniform samplerCube cubemap;

uniform bool hdr;

void main() {
    float mipLevel = 0;
    float gamma = 2.2;

    vec3 envColor = textureLod(cubemap, localPos, mipLevel).rgb;

    if (hdr) {
        // Tone map
        envColor = envColor / (envColor + vec3(1));
        // Gamma correct
        envColor = pow(envColor, vec3(1.0)/gamma);
    }

    FragColor = vec4(envColor, 1);
}