#version 410 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D rawScene;

void main() {
    vec3 color = texture(rawScene, TexCoords).rgb;
    
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0)
        FragColor = vec4(smoothstep(0.825, 1.175, brightness) * color, 1.0);  // Smoothstep prevents harsh bloom boundary
    else
        FragColor = vec4(0, 0, 0, 1.0);
}
