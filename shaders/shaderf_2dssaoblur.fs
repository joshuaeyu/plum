#version 410 core

in vec2 TexCoords;
out float FragColor;

uniform sampler2D ssaoInput;

void main() {
    vec2 texelStep = 1.0 / vec2(textureSize(ssaoInput, 0));
    // Assuming 4x4 noise texture buffer (16 pixels)
    float result = 0;
    float samples_x = 4;  // Match noise texture size
    float samples_y = 4;  // Match noise texture size
    for (float x = -(samples_x-1)/2; x <= (samples_x-1)/2; x++) {
        for (float y = -(samples_y-1)/2; y <= (samples_y-1)/2; y++) {
            vec2 offset = vec2(x,y) * texelStep;
            result += texture(ssaoInput, TexCoords + offset).r;
        }
    }
    FragColor = result / (samples_x * samples_y);
}