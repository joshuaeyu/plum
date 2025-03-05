#pragma once

#include <plum/core/core.hpp>
#include <plum/scene/scenenode.hpp>

#include <plum/material/material.hpp>
#include <plum/material/module.hpp>

#include <vector>
#include <memory>

namespace Component {

    class Mesh : public Component {
        // SceneNode has Draw(), transform, parent, children
        
        public:
            Mesh();
            Mesh(Core::Vao& vao);
            Mesh(Core::Vao& vao, Material::Material& mat);
            virtual ~Mesh();
            
            virtual void Draw(const glm::mat4& model_matrix = glm::identity<glm::mat4>());
            virtual void Draw(Material::Material& mat, const glm::mat4& model_matrix = glm::identity<glm::mat4>());
            virtual void Draw(Material::Module& module, const glm::mat4& model_matrix = glm::identity<glm::mat4>());
            
            std::shared_ptr<Material::Material> material = std::make_shared<Material::PBRMetallicMaterial>();
            
            std::shared_ptr<Core::Vao> vao;
            
        protected:
            Mesh(const ComponentType type);

    };

}