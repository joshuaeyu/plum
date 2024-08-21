#version 330 core

in vec3 vfPosition;
in vec3 vfNormal;
in vec2 vfTexCoords;

uniform sampler2D tex;
uniform samplerCube cubemap;
uniform vec3 cameraPos;

out vec4 FragColor;

void main() {
    float refractRatio = 1.00 / 1.52;
    vec3 incidenceDir = normalize(vfPosition - cameraPos);
    vec3 reflectDir = reflect(incidenceDir, normalize(vfNormal));
    vec3 refractDir = refract(incidenceDir, normalize(vfNormal), refractRatio);
    FragColor = texture(cubemap, reflectDir);
    FragColor = texture(cubemap, refractDir);
}