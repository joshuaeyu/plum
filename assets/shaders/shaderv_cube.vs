#version 330 core

// INPUTS (VAA)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

// OUTPUTS
out vec3 vfPosition;
out vec3 vfNormal;
out vec2 vfTexCoords;

// TRANSFORMS
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vfNormal = mat3(transpose(inverse(model))) * aNormal;
    vfPosition = vec3( model * vec4(aPos, 1.0) );    // 3D world position
    vfTexCoords = aTexCoord;
    gl_Position = projection * view * model * vec4(aPos, 1.0);  // 2D screen position
}