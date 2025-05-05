#version 410 core

// shaderf_basichybrid.fs
// Geometry pass fragment shader for surfaces with raw albedo/metalness/roughness and textures
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
uniform struct Material_PBRMetallic {
    // Textures
    sampler2D texture_albedo;
    sampler2D texture_metallic;
    sampler2D texture_roughness;
    sampler2D texture_normal;
    sampler2D texture_height;
    sampler2D texture_occlusion;
    bool has_texture_albedo;
    bool has_texture_metallic;
    bool has_texture_roughness;
    bool has_texture_normal;
    bool has_texture_height;
    bool has_texture_occlusion;
    // Raw
    vec3 albedo;
    float metallic;
    float roughness;

} material;

// ============== MAIN ==============
void main() {
    gPosition = fs_in.FragPos;

    if (material.has_texture_normal) {
        gNormal = texture(material.texture_normal, fs_in.TexCoords).rgb;
        gNormal = gNormal * 2.0 - 1.0;
        gNormal = normalize(fs_in.TBN * gNormal);
    } else {
        gNormal = normalize(fs_in.Normal);
    }

    gAlbedoSpec.rgb = material.albedo;
    if (material.has_texture_albedo) {
        gAlbedoSpec.rgb += texture(material.texture_albedo, fs_in.TexCoords).rgb;
    }

    if (material.has_texture_metallic) {
        gMetRouOcc.r = texture(material.texture_metallic, fs_in.TexCoords).b;   // .gltf
    } else {
        gMetRouOcc.r = material.metallic;
    }

    if (material.has_texture_roughness) {
        gMetRouOcc.g = texture(material.texture_roughness, fs_in.TexCoords).g;  // .gltf
    } else {
        gMetRouOcc.g = material.roughness;
    }

    if (material.has_texture_occlusion) {
        gMetRouOcc.b = 1.0; // aka, use occlusion texture
        gMetRouOcc.a = texture(material.texture_occlusion, fs_in.TexCoords).r;  // .gltf
    } else {
        gMetRouOcc.b = 0.0; // aka, use SSAO
        gMetRouOcc.a = 1.0;
    }
}
// ==================================

// To be implemented in lighting pass
    // // Other properties
    // float shininess;
    // float opacity;