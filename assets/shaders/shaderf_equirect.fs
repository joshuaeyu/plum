#version 410 core

in vec3 localPos;
out vec4 FragColor;

uniform sampler2D equirectMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 pos) {
    vec2 uv = vec2( atan(pos.z, pos.x), asin(pos.y) );
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main() {
    vec2 uv = SampleSphericalMap( normalize(localPos) );
    FragColor = texture(equirectMap, uv);
    // FragColor = vec4(localPos+0.5,1);
}