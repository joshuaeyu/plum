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

}