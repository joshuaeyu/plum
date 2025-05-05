#version 330 core

layout (points) in;     // from vertex shader
layout (triangle_strip, max_vertices = 5) out;  // to fragment shader
// layout (line_strip, max_vertices = 2) out;  // to fragment shader

in VS_OUT {
    vec3 color;
} gs_in[];

out vec3 fColor;

void buildhouse(vec4 position) {
    fColor = gs_in[0].color;
    gl_Position = position + vec4(-0.2,-0.2,0,0);
    EmitVertex();
    gl_Position = position + vec4(0.2,-0.2,0,0);
    EmitVertex();
    gl_Position = position + vec4(-0.2,0.2,0,0);
    EmitVertex();
    gl_Position = position + vec4(0.2,0.2,0,0);
    EmitVertex();
    gl_Position = position + vec4(0.0,0.4,0,0);
    fColor = vec3(1);
    EmitVertex();
    EndPrimitive();
}

void drawlines(vec4 position) {
    gl_Position = gl_in[0].gl_Position + vec4(-0.1,0,0,0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.1,0,0,0);
    EmitVertex();

    EndPrimitive();
}

void main() {
    buildhouse(gl_in[0].gl_Position);
}

