#pragma once

#include <plum/scene/mesh.hpp>

namespace Component {
    class Model : public Mesh {
        public:
            
            Model();
            void Draw(const glm::mat4& model_matrix = glm::identity<glm::mat4>());
            void Draw(Material::Material& mat, const glm::mat4& model_matrix = glm::identity<glm::mat4>());
            void Draw(Material::Module& module, const glm::mat4& model_matrix = glm::identity<glm::mat4>());

            // bones and animation

            std::vector<std::shared_ptr<Mesh>> meshes;
    };
}