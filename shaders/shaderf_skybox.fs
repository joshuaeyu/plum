#version 410 core

in vec3 localPos;
out vec4 FragColor;

uniform samplerCube cubemap;

void main() {
    float mipLevel = 0;
    float gamma = 2.2;

    vec3 envColor = textureLod(cubemap, localPos, mipLevel).rgb;

    FragColor = vec4(envColor, 1);
}