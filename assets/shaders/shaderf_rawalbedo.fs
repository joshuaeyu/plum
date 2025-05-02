#version 410 core

// shaderf_floor.fs
// Fragment shader for basic geometry
// Details: Uses external textures as loaded by model class
// Space: Viewspace

// OUTPUTS
layout (location = 0) out vec4 FragColor;   // GL_COLOR_ATTACHMENT0 from glDrawBuffers
layout (location = 1) out vec4 BrightColor; // GL_COLOR_ATTACHMENT1 from glDrawBuffers

// MATERIAL
uniform struct Material {
    // Colors
    vec3 albedo;
    // Other properties
    float opacity;
} material;

// ============== MAIN ==============
void main() {
    // Output to two colorbuffers
    FragColor = vec4(material.albedo, 1.0);
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0, 0, 0, 1.0);
}
// ==================================