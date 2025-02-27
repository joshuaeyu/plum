#pragma once

#include <vector>
#include <memory>

#include <plum/component/core.hpp>
#include <plum/component/scenenode.hpp>
#include <plum/material/material.hpp>
#include <plum/material/module.hpp>

namespace Component {

    class Mesh : public SceneObject {
        // SceneNode has Draw(), transform, parent, children
        
        public:
            Mesh();
            Mesh(std::shared_ptr<Vao> va);
            virtual ~Mesh();
            
            virtual void Draw(const glm::mat4& model_matrix = glm::identity<glm::mat4>());
            virtual void Draw(Material::Material& material, const glm::mat4& model_matrix = glm::identity<glm::mat4>());
            virtual void Draw(Material::Module& module, const glm::mat4& model_matrix = glm::identity<glm::mat4>());
            
            std::shared_ptr<Material::Material> material;
            // friend class Renderer;
            
            std::shared_ptr<Vao> vao;
            
        protected:
            Mesh(const SceneObjectType type);

    };

}