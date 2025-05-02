#version 410 core

// shaderv_gen.vs
// Geometry pass vertex shader for most cases
// Details: Usable for .3mf, .obj, raw vertex data, etc.
// Output space: Viewspace

// INPUTS (VAA)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in mat4 aModel;

// TRANSFORMS
layout (std140) uniform Matrices_Vertex {  // this is 2 * sizeof(glm::mat4)
    mat4 view;
    mat4 projection;
};
uniform mat4 model;

// OUTPUTS
out VS_OUT {
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    mat3 TBN;
} vs_out;

void main() {
    // Core parameters
    vs_out.FragPos = view * model * vec4(aPos, 1.0);
    vs_out.Normal = mat3(transpose(inverse(view * model))) * aNormal;    // normal_matrix * aNormal
    vs_out.TexCoords = aTexCoord;
    vec3 N = normalize(vec3(view * model * vec4(aNormal, 0)));
    vec3 T = normalize(vec3(view * model * vec4(aTangent, 0)));
    // vec3 B = normalize(vec3(view * model * vec4(aBitangent, 0)));
    T = normalize(T - dot(T,N) * N);    // Reorthogonalize T wrt N
    vec3 B = cross(N, T);
    vs_out.TBN = mat3(T, B, N);
    
    // Clip space position
    gl_Position = projection * vs_out.FragPos;
}

// To be implemented in lighting pass

    // vec3 FragPosWorld;          // Not needed in geometry pass // Can just use inverse view matrix
    // vec3 CameraPos;             // Not needed in geometry pass // Should be (0,0,0)
    // vec3 DirecLightDir;         // Not needed in geometry pass // Can just use  transpose inverse view matrix
    // vec3 PointLightPos[8];      // Not needed in geometry pass // Can pass in directly
    // vec3 PointLightPosWorld[8]; // Not needed in geometry pass // Can just use inverse view matrix
    // vec4 FragPosLightSpace[8];  // Not needed in geometry pass // Can just use lightspace matrix

// layout (std140) uniform Camera {  // this is 2 * sizeof(glm::mat4)
//     vec3 position;
//     vec3 front;
// };

// // LIGHTING
// layout (std140) uniform DirLight_Vertex {
//     vec3 dirlight_direction;
// };
// layout (std140) uniform PointLight_Vertex {
//     int pointlight_count;
//     vec3 pointlight_position[8];
// };
// layout (std140) uniform Shadows_Vertex {
//     mat4 lightspace_matrix[8];
// };

    // vs_out.FragPosWorld = vec3(model * vec4(aPos, 1.0));
    // // Camera parameters
    // vs_out.CameraPos = vec3(view * vec4(position, 1.0)); // same as myCameraPos = vec3(0);
    // // Light parameters
    // vs_out.DirecLightDir = mat3(transpose(inverse(view))) * dirlight_direction;   // normal_matrix * direction
    // for (int j = 0; j < 8; j++) {
    //     vs_out.PointLightPos[j] = vec3(view * vec4(pointlight_position[j], 1.0));
    //     vs_out.PointLightPosWorld[j] = pointlight_position[j];
    //     vs_out.FragPosLightSpace[j] = lightspace_matrix[j] * model * vec4(aPos, 1.0);
    // }