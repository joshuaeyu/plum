#version 410 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far;

void main() {
    float lightDistance = distance(FragPos.xyz, lightPos); // both in worldspace
    lightDistance /= far;
    gl_FragDepth = lightDistance;
}