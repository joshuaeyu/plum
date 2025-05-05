#version 410 core

// shaderf_3mf.fs
// Geometry pass fragment shader for .3mf files
// Details: Only uses $clr.diffuse
// Space: Viewspace

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
uniform struct Material_3MF {
    vec3 color_diffuse;
} material;

// ============== MAIN ==============
void main() {
    gPosition = fs_in.FragPos;
    gNormal = normalize(fs_in.Normal);
    gAlbedoSpec.rgb = material.color_diffuse;
    gAlbedoSpec.a = 0;
    gMetRouOcc = vec4(0.0, 0.175, 0, 0);
}
// ==================================