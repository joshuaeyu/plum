#version 410 core

in vec2 TexCoords;

out vec4 FragColor;

uniform float intensity = 1.0;
uniform sampler2D sceneRaw;
uniform sampler2D sceneBloom;

void main() {
    // Additively blend HDR and bloom color values
    vec3 rawColor = texture(sceneRaw, TexCoords).rgb;
    vec3 bloomColor = texture(sceneBloom, TexCoords).rgb;
    FragColor = vec4(rawColor + intensity*bloomColor, 1);
}
