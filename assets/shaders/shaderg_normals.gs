#version 330 core

layout (triangles) in;     // from vertex shader
layout (line_strip, max_vertices = 9) out;  // to fragment shader

// INPUTS
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 DirecLightDir;
} gs_in[];

uniform mat4 projection;

// OUTPUTS
out GS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 DirecLightDir;
} gs_out;

// HELPER FUNCTIONS
void GenerateLine(int index) {
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position + vec4(normalize(gs_in[index].Normal),0.0) * 0.3);
    EmitVertex();
    EndPrimitive();
}

// ====== MAIN =====
void main() {
    for (int i = 0; i < 3; i++) {
        // Calculate new position
        GenerateLine(i);
        // // Update gs_out
        // gs_out.FragPos       = vec3(projection * gl_in[i].gl_Position);
        // gs_out.Normal        = gs_in[i].Normal;
    }
    // // Direc light
    // gl_Position = projection * gl_in[0].gl_Position;
    // EmitVertex();
    // gl_Position = projection * (gl_in[0].gl_Position - vec4(normalize(gs_in[0].DirecLightDir), 0.0) * 0.7);
    // EmitVertex();
    // EndPrimitive();
}

