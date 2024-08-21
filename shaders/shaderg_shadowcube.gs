#version 410 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform int layer;
uniform mat4 lightSpaceMatrices[6];

out vec4 FragPos;

void func(int i) {
    gl_Layer = 6*layer + i;   // Built-in, only for when rendering to a framebuffer with a cubemap attached - MAY NEED STATIC ASSIGNMENT FOR THIS
    for (int j = 0; j < 3; j++) {
        FragPos = gl_in[j].gl_Position;
        gl_Position = lightSpaceMatrices[i] * FragPos;  // Transform each triangle into the light space of each cube face
        EmitVertex();
    }
    EndPrimitive();
}

void main() {
    func(0);
    func(1);
    func(2);
    func(3);
    func(4);
    func(5);
}