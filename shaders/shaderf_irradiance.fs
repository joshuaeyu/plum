#version 410 core

in vec3 localPos;
out vec4 FragColor;

uniform samplerCube envCubemap;

const float PI = 3.14159265359;

void main() {
    // Orthonormal basis vectors for world space
    vec3 normal = normalize(localPos);  // Based on view matrix supplied in vertex shader
    vec3 up = vec3(0.0,1.0,0.0);
    vec3 right = normalize(cross(up, normal));  
    up = normalize(cross(normal,right));
    mat3 tangentToWorld = mat3(right, up, normal);

    // Riemann sum to compute irradiance
    vec3 irradiance = vec3(0.0);
    float stepSize = 0.025;
    int numSamples = 0;
    for (float phi = 0.0; phi < 2.0 * PI; phi += stepSize) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += stepSize) {
            // Tangent space (independent of view matrix)
            vec3 tangentVec = vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)); 
            // Change of basis to world space
            vec3 sampleVec = tangentToWorld * tangentVec;
            irradiance += texture(envCubemap, sampleVec).rgb * cos(theta) * sin(theta);
            numSamples++;
        }
    }

    irradiance *= PI / float(numSamples);
    
    FragColor = vec4(irradiance, 1.0);
}