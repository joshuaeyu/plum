#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>
    
struct Vertex {
    glm::vec3 position;    // Generally for compile- or run-time data
    glm::vec3 normal;      // Generally for compile- or run-time data
    glm::vec2 uv;           // Generally for compile- or run-time data
    glm::vec3 tangent;
    glm::vec3 bitangent;
};  

#endif