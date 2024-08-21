#version 410 core

in vec2 TexCoords;

layout (location = 0) out vec4 RawColor;   // GL_COLOR_ATTACHMENT0 from glDrawBuffers
layout (location = 1) out vec4 BlurredColor;     // GL_COLOR_ATTACHMENT1 from glDrawBuffers

uniform sampler2D image;

uniform bool horizontal;
float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
    vec2 tex_offset = 1.0 / textureSize(image, 0);
    vec3 result = texture(image, TexCoords).rgb * weight[0];
    if (horizontal) {
        for (int i = 1; i < 5; i++) {
            result += texture(image, TexCoords + i * vec2(tex_offset.x,0)).rgb * weight[i];
            result += texture(image, TexCoords - i * vec2(tex_offset.x,0)).rgb * weight[i];
        }
    } else {
        for (int i = 1; i < 5; i++) {
            result += texture(image, TexCoords + i * vec2(0,tex_offset.y)).rgb * weight[i];
            result += texture(image, TexCoords - i * vec2(0,tex_offset.y)).rgb * weight[i];
        }
    }
    BlurredColor = vec4(result, 1);
}
