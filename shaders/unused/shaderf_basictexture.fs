#version 410 core

// shaderf_basictexture.fs
// Geometry pass fragment shader for surfaces with textures only
// Input space: Viewspace
// Output space: Viewspace

// OUTPUTS
layout (location = 0) out vec4 gPosition;   // GL_COLOR_ATTACHMENT0 from glDrawBuffers
layout (location = 1) out vec3 gNormal;     // GL_COLOR_ATTACHMENT1 from glDrawBuffers
layout (location = 2) out vec4 gAlbedoSpec; // GL_COLOR_ATTACHMENT2 from glDrawBuffers
layout (location = 3) out vec4 gMetRouOcc;  // GL_COLOR_ATTACHMENT3

// INPUTS
in VS_OUT {
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

// MATERIAL
uniform struct Material_BASICTEXTURE {
    // Textures
    sampler2D texture_ambient;
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D texture_height;
    sampler2D texture_normal;
    sampler2D texture_metalness;
    sampler2D texture_roughness;
    sampler2D texture_occlusion;
    sampler2D texture_unknown;
    int texture_ambient_count;
    int texture_diffuse_count;
    int texture_specular_count;
    int texture_height_count;
    int texture_normal_count;
    int texture_metalness_count;
    int texture_roughness_count;
    int texture_occlusion_count;    
    int texture_unknown_count;
} material;

// ============== MAIN ==============
void main() {
    gPosition = fs_in.FragPos;

    if (material.texture_height_count > 0) {
        gNormal = texture(material.texture_height, fs_in.TexCoords).rgb;
        gNormal = gNormal * 2.0 - 1.0;
        gNormal = normalize(fs_in.TBN * gNormal);
    } else {
        gNormal = normalize(fs_in.Normal);
    }

    gAlbedoSpec.rgb = texture(material.texture_diffuse, fs_in.TexCoords).rgb;

    if (material.texture_specular_count > 0) {
        gAlbedoSpec.a = texture(material.texture_specular, fs_in.TexCoords).r;
    } else {
        gAlbedoSpec.a = 0;
    }

    if (material.texture_metalness_count > 0) {
        gMetRouOcc.r = texture(material.texture_metalness, fs_in.TexCoords).r;
    } else {
        gMetRouOcc.r = 0.25;
    }

    if (material.texture_roughness_count > 0) {
        gMetRouOcc.g = texture(material.texture_roughness, fs_in.TexCoords).r;
    } else {
        gMetRouOcc.g = 0.75;
    }

    if (material.texture_occlusion_count > 0) {
        gMetRouOcc.b = 1.0; // aka, ignore and use SSAO
        gMetRouOcc.a = texture(material.texture_occlusion, fs_in.TexCoords).r;
    } else {
        gMetRouOcc.b = 0.0; // aka, use SSAO
    }
}
// ==================================

// To be implemented in lighting pass
    // // Other properties
    // float shininess;
    // float opacity;