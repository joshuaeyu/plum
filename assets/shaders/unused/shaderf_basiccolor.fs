#version 410 core

// shaderf_basiccolor.fs
// Geometry pass fragment shader surfaces with colors only
// Input space: Viewspace
// Output space: Viewspace

// OUTPUTS
layout (location = 0) out vec4 gPosition;   // GL_COLOR_ATTACHMENT0 from glDrawBuffers
layout (location = 1) out vec3 gNormal;     // GL_COLOR_ATTACHMENT1 from glDrawBuffers
layout (location = 2) out vec4 gAlbedoSpec; // GL_COLOR_ATTACHMENT2 from glDrawBuffers
layout (location = 3) out vec4 gMetRouOcc;     // GL_COLOR_ATTACHMENT3

// INPUTS
in VS_OUT {
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

// MATERIAL
uniform struct Material_BASICCOLOR {
    vec3 albedo;
    float metallic;
    float roughness;
} material; 

// ============== MAIN ==============
void main() {
    gPosition = fs_in.FragPos;
    gNormal = normalize(fs_in.Normal);
    gAlbedoSpec.rgb = material.albedo;
    gAlbedoSpec.a = material.metallic;
    gMetRouOcc = vec4(material.metallic, material.roughness, 0.0, 0.0);
}
// ==================================

// To be implemented in lighting pass
    // Other properties
    // float shininess;
    // float opacity;