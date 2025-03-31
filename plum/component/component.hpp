#pragma once

#include <plum/material/material.hpp>
#include <plum/renderer/module.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Component {
            
    inline static const glm::vec3 UP = glm::vec3(0,1,0);
    inline static const glm::vec3 DOWN = glm::vec3(0,-1,0);
    inline static const glm::vec3 LEFT = glm::vec3(-1,0,0);
    inline static const glm::vec3 RIGHT = glm::vec3(1,0,0);
    inline static const glm::vec3 FRONT = glm::vec3(0,0,1);
    inline static const glm::vec3 BACK = glm::vec3(0,0,-1);

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

            const ComponentType type;
            
            const bool IsMesh() const;
            const bool IsLight() const;
        
        protected:
            ComponentBase(const ComponentType type);
    };

}