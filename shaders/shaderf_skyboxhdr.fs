#version 410 core

in vec3 localPos;
out vec4 FragColor;

uniform samplerCube cubemap;

void main() {
    float mipLevel = 0;
    float gamma = 2.2;

    vec3 envColor = texture(cubemap, localPos, mipLevel).rgb;

    // Tone map
    envColor = envColor / (envColor + vec3(1));
    // // Gamma correct
    envColor = pow(envColor, vec3(1.0)/gamma);

    FragColor = vec4(envColor, 1);
}