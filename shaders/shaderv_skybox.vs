#version 410 core

layout (location = 0) in vec3 aPos;

out vec3 localPos;

uniform mat4 projection;
uniform mat4 view;

void main() {
    localPos = aPos;

    // Remove translation part of view matrix
    gl_Position = projection * mat4(mat3(view)) * vec4(aPos, 1.0);
    gl_Position = gl_Position.xyww;
}