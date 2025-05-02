#version 410 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D hdrBuffer;
uniform float exposure = 1.0;

const float GAMMA = 2.2;

void main() {
    // Exposure tone mapping
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    vec3 mappedColor = vec3(1) - exp(-hdrColor * exposure);
    // Gamma correction
    mappedColor = pow(mappedColor, vec3(1.0/GAMMA));
    FragColor = vec4(mappedColor, 1);
}
