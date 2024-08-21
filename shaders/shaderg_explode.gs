#version 330 core

layout (triangles) in;     // from vertex shader
layout (triangle_strip, max_vertices = 3) out;  // to fragment shader

// INPUTS
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 CameraPos;
    vec3 DirecLightDir;
} gs_in[];
uniform float time;

// OUTPUTS
out GS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 CameraPos;
    vec3 DirecLightDir;
} gs_out;

// HELPER FUNCTIONS
vec3 GetTriangleNormal() {
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a,b));
}
vec4 Explode(vec4 position, vec3 normal) {
    float magnitude = 2.0;
    vec3 direction = normal * (sin(time/2.0) + 1.0) * magnitude;
    return position + vec4(direction, 0);
}

// ====== MAIN =====
void main() {
    vec3 normal = GetTriangleNormal();
    for (int i = 0; i < 3; i++) {
        // Calculate new position
        // gl_Position = Explode(gl_in[i].gl_Position, normal);
        gl_Position = Explode(gl_in[i].gl_Position, gs_in[i].Normal);
        
        // Update gs_out
        gs_out.FragPos       = vec3(gl_Position);
        gs_out.TexCoords     = gs_in[i].TexCoords;
        gs_out.Normal        = gs_in[i].Normal;
        gs_out.CameraPos     = gs_in[i].CameraPos;
        gs_out.DirecLightDir = gs_in[i].DirecLightDir;

        // Emit
        EmitVertex();
    }
    EndPrimitive();
}

