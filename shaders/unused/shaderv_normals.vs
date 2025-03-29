#version 330 core

// INPUTS (VAA)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

// TRANSFORMS
layout (std140) uniform Matrices {  // this is 2 * sizeof(glm::mat4)
    mat4 view;
    mat4 projection;
};
uniform mat4 model;

// LIGHTING
layout (std140) uniform DirLight_Vertex {
    vec3 dirlight_direction;
};

// OUTPUTS
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 DirecLightDir;
} vs_out;

void main() {
    gl_Position = view * model * vec4(aPos, 1.0);
    vs_out.Normal = mat3(transpose(inverse(view * model))) * aNormal;    // normal_matrix * aNormal
    vs_out.FragPos = vec3(view * model * vec4(aPos, 1.0));
    vs_out.DirecLightDir = mat3(transpose(inverse(view))) * dirlight_direction;   // normal_matrix * direction
}