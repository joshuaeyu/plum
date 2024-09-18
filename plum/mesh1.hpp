#include <plum/vertex.hpp>
#include <vector>

namespace Component {

    class Mesh {
        private:
            std::vector<Component::Vertex> vertices;
            std::vector<Component::Index> indices;

            // vao and ebo instead
        public:
            void draw(/*glcore instance*/) {
                // instance.draw(this)
            }
            Mesh() {}
        protected:
    };

}