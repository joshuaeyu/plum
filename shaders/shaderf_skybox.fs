#version 410 core

in vec3 localPos;
out vec4 FragColor;

uniform samplerCube cubemap;

void main() {
    FragColor = texture(cubemap, localPos);
}