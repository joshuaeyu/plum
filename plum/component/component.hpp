#pragma once

#include <plum/material/material.hpp>
#include <plum/renderer/module.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Component {

    enum class ComponentType {
        // Meshes
        Mesh,
        Model,
        ModelNode,
        Primitive,
        // Lights
        DirLight,
        PointLight
    };

    class ComponentBase {
        public: 
            virtual ~ComponentBase();
            
            virtual void Draw(const glm::mat4& model_matrix = glm::identity<glm::mat4>()) {}
            virtual void Draw(Material::MaterialBase& material, const glm::mat4& model_matrix = glm::identity<glm::mat4>()) {}
            virtual void Draw(Renderer::Module& module, const glm::mat4& model_matrix = glm::identity<glm::mat4>()) {}

            const bool IsMesh() const;
            const bool IsLight() const;

            virtual void DisplayWidget() {}
            
            const ComponentType type;
            std::string name = "Component";
        
        protected:
            ComponentBase(const ComponentType type);
    };

}