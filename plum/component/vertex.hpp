#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>    

namespace Component {

    class Vertex {
        public:
            
            static std::vector<float> Zip(std::vector<Vertex>& vertices);
            
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 uv;
            glm::vec3 tangent;
            glm::vec3 bitangent;
            
            Vertex() {}
            
        protected:
        private:
    };

    class Index {
        public:
            unsigned int value;
            
            Index() {}
        protected:
        private:
    };

}

#endif