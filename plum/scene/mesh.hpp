#pragma once

#include <plum/component/core.hpp>
#include <plum/scene/scenenode.hpp>

#include <plum/material/material.hpp>
#include <plum/material/module.hpp>

#include <vector>
#include <memory>

namespace Component {

    class Mesh : public SceneObject {
        // SceneNode has Draw(), transform, parent, children
        
        public:
            Mesh();
            Mesh(Vao& vao);
            Mesh(Vao& vao, Material::Material& mat);
            virtual ~Mesh();
            
            virtual void Draw(const glm::mat4& model_matrix = glm::identity<glm::mat4>());
            virtual void Draw(Material::Material& mat, const glm::mat4& model_matrix = glm::identity<glm::mat4>());
            virtual void Draw(Material::Module& module, const glm::mat4& model_matrix = glm::identity<glm::mat4>());
            
            std::shared_ptr<Material::Material> material = std::make_shared<Material::PBRMetallicMaterial>();
            
            std::shared_ptr<Vao> vao;
            
        protected:
            Mesh(const SceneObjectType type);

    };

}