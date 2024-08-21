#version 410 core

out vec4 FragColor;

in vec2 TexCoords;

// uniform sampler2D depthmap;
uniform sampler2DArray depthmap_array;
uniform float far_plane; // for perspective only

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    float near_plane = 0.1;
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{             
    // float depthValue = texture(depthmap, TexCoords).r;
    float depthValue = texture(depthmap_array, vec3(TexCoords,0)).r;
    // FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
    FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}  

// void main() {
//     float z = texture(depthmap, TexCoords).r;
//     FragColor = vec4(vec3(z), 1.0);
// }