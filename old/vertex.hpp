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

namespace Component {

    class Vertex {
        private:
        public:
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 uv;
            glm::vec3 tangent;
            glm::vec3 bitangent;
            
            Vertex() {}
            
        protected:
    };

    class Index {
        private:
        public:
            unsigned int value;
            
            Index() {}
        protected:
    };

}

#endif