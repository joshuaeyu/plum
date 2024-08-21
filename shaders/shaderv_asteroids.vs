#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 instanceModelMatrix;

// TRANSFORMS
layout (std140) uniform Matrices {  // this is 2 * sizeof(glm::mat4)
    mat4 view;
    mat4 projection;
};
layout (std140) uniform Camera {  // this is 2 * sizeof(glm::mat4)
    vec3 position;
    vec3 front;
};

// LIGHTING
layout (std140) uniform DirLight_Vertex {
    vec3 dirlight_direction;
};

// OUTPUTS
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 CameraPos;
    vec3 DirecLightDir;
} vs_out;

void main() {
    gl_Position = projection * view * instanceModelMatrix * vec4(aPos, 1.0);
    vs_out.Normal = mat3(transpose(inverse(view * instanceModelMatrix))) * aNormal;    // normal_matrix * aNormal
    vs_out.FragPos = vec3(view * instanceModelMatrix * vec4(aPos, 1.0));
    vs_out.CameraPos = vec3(view * vec4(position, 1.0)); // same as myCameraPos = vec3(0);
    vs_out.DirecLightDir = mat3(transpose(inverse(view))) * dirlight_direction;   // normal_matrix * direction
    vs_out.TexCoords = aTexCoord;
}