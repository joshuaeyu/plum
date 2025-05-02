#version 330 core

in GS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 DirecLightDir;
} fs_in;

out vec4 FragColor;

void main() {
    FragColor = vec4(0.5,0.5,0,0);
}