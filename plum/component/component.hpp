#pragma once

#include <plum/material/material.hpp>
#include <plum/material/module.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Component {
            
    enum class ComponentType {
        // Meshes
        Mesh,
        Model,
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
            virtual void Draw(Material::Module& module, const glm::mat4& model_matrix = glm::identity<glm::mat4>()) {}

            const ComponentType type;
            
            const bool IsMesh() const;
            const bool IsLight() const;
        
        protected:
            ComponentBase(const ComponentType type);
    };

}