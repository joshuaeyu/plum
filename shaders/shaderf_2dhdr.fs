#version 410 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D hdrBuffer;
uniform float exposure;

void main() {
    // Exposure tone mapping
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    vec3 mappedColor = vec3(1) - exp(-hdrColor * exposure);
    mappedColor = pow(mappedColor, vec3(1.0/gamma));
    FragColor = vec4(mappedColor, 1);
}
