#version 330 core

layout (triangles) in;     // from vertex shader
layout (triangle_strip, max_vertices = 3) out;  // to fragment shader

// INPUTS
in VS_OUT {
    vec3 FragPos;
    vec3 FragPosWorld;
    vec3 Normal;
    vec2 TexCoords;
    vec3 CameraPos;
    vec3 DirecLightDir;
    vec3 PointLightPos[8];
    vec3 PointLightPosWorld[8];
    vec4 FragPosLightSpace[8];
} gs_in[];
uniform float time;

// OUTPUTS
out GS_OUT {
    vec3 FragPos;
    vec3 FragPosWorld;
    vec3 Normal;
    vec2 TexCoords;
    vec3 CameraPos;
    vec3 DirecLightDir;
    vec3 PointLightPos[8];
    vec3 PointLightPosWorld[8];
    vec4 FragPosLightSpace[8];
} gs_out;

// ====== MAIN =====
void main() {
    for (int i = 0; i < 3; i++) {
        // Pass everything through
        gl_Position          = gl_in[i].gl_Position;
        gs_out.FragPos       = gs_in[i].FragPos;
        gs_out.FragPosWorld  = gs_in[i].FragPosWorld;
        gs_out.TexCoords     = gs_in[i].TexCoords;
        gs_out.Normal        = gs_in[i].Normal;
        gs_out.CameraPos     = gs_in[i].CameraPos;
        gs_out.DirecLightDir = gs_in[i].DirecLightDir;

        for (int j = 0; j < 8; j++) {
            gs_out.PointLightPos[j]      = gs_in[i].PointLightPos[j];            
            gs_out.PointLightPosWorld[j] = gs_in[i].PointLightPosWorld[j];            
            gs_out.FragPosLightSpace[j]  = gs_in[i].FragPosLightSpace[j];
        }
        // Emit
        EmitVertex();
    }
    EndPrimitive();
}

