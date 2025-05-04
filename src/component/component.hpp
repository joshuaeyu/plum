#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Material {
    class MaterialBase;
};

namespace Renderer {
    class RenderModule;
};

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
            
            const ComponentType type;
            std::string name = "Component";
            
            const bool IsMesh() const;
            const bool IsLight() const;

            virtual void Draw(const glm::mat4& model_matrix = glm::identity<glm::mat4>()) {}
            virtual void Draw(Material::MaterialBase& material, const glm::mat4& model_matrix = glm::identity<glm::mat4>()) {}
            virtual void Draw(Renderer::RenderModule& module, const glm::mat4& model_matrix = glm::identity<glm::mat4>()) {}
            virtual std::shared_ptr<ComponentBase> Duplicate() = 0;
            
            // GUI widget
            virtual void DisplayWidget() {}            
        
        protected:
            ComponentBase(const ComponentType type);
    };

}