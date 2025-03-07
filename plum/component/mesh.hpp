#pragma once

#include <plum/core/core.hpp>
#include <plum/scene/scenenode.hpp>

#include <plum/material/material.hpp>
#include <plum/material/module.hpp>

#include <vector>
#include <memory>

namespace Component {

    class Mesh : public ComponentBase {
        
        public:
            Mesh();
            Mesh(std::shared_ptr<Core::Vao> vao);
            Mesh(std::shared_ptr<Core::Vao> vao, std::shared_ptr<Material::MaterialBase> mat);
            virtual ~Mesh();
            
            virtual void Draw(const glm::mat4& model_matrix = glm::identity<glm::mat4>());
            virtual void Draw(Material::MaterialBase& mat, const glm::mat4& model_matrix = glm::identity<glm::mat4>());
            virtual void Draw(Material::Module& module, const glm::mat4& model_matrix = glm::identity<glm::mat4>());
            
            std::shared_ptr<Material::MaterialBase> material = std::make_shared<Material::PBRMetallicMaterial>();
            
            std::shared_ptr<Core::Vao> vao;
            
        protected:
            Mesh(const ComponentType type);

    };

}