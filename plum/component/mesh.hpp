#ifndef MESH_HPP
#define MESH_HPP

#include <plum/vertex.hpp>
#include <plum/core/core.hpp>
#include <vector>

namespace Component {

    class Mesh {
        
        public:
            Mesh();
            Mesh(std::shared_ptr<Core::Vao> vao);
            Mesh(std::vector<Component::Vertex>& vertices, std::vector<Component::Index>& indices);
            ~Mesh();

            void Draw() const;
            
        protected:
            std::shared_ptr<std::vector<Vertex>> vertices;
            std::shared_ptr<std::vector<Index>> indices;
            std::shared_ptr<Core::Vao> vao;
            std::shared_ptr<Core::Vbo> vbo;
            std::shared_ptr<Core::Ebo> ebo;

        private:
    };

// need something to manage gl core objects and their actual array data
// Mesh aggregates gl objects and ___ and allows high level operations on them
// check if vertices already have associated vao vbo ebo etc.

// new Mesh()
// new

}

#endif