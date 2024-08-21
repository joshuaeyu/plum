#version 410 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D sceneRaw;
uniform sampler2D sceneBloom;

void main() {
    // Additively blend HDR and bloom color values
    float intensity = 0.5;
    vec3 hdrColor = texture(sceneRaw, TexCoords).rgb;
    vec3 bloomColor = texture(sceneBloom, TexCoords).rgb;
    FragColor = vec4(hdrColor + intensity*bloomColor, 1);
}
