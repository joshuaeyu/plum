#pragma once

#include <plum/material/material.hpp>
#include <plum/material/module.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Component {

    class Component {
        public: 
            virtual ~Component();
            
            virtual void Draw(const glm::mat4& model_matrix = glm::identity<glm::mat4>()) {}
            virtual void Draw(Material::Material& material, const glm::mat4& model_matrix = glm::identity<glm::mat4>()) {}
            virtual void Draw(Material::Module& module, const glm::mat4& model_matrix = glm::identity<glm::mat4>()) {}
            
            enum class ComponentType {
                // Meshes
                Mesh,
                Model,
                Primitive,
                // Lights
                DirLight,
                PointLight
            };
            
            const ComponentType type;
            
            const bool IsMesh() const;
            const bool IsLight() const;
        
        protected:
            Component(const ComponentType type);
    };

}