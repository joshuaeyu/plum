#pragma once

#include <plum/component/component.hpp>

#include <plum/core/globject.hpp>
#include <plum/material/material.hpp>
#include <plum/renderer/module.hpp>
#include <plum/scene/scenenode.hpp>

#include <memory>

namespace Component {

    class Mesh : public ComponentBase {
        
        public:
            Mesh();
            Mesh(std::shared_ptr<Core::Vao> vao);
            Mesh(std::shared_ptr<Core::Vao> vao, std::shared_ptr<Material::MaterialBase> material);
            virtual ~Mesh();
            
            virtual void Draw(const glm::mat4& model_matrix = glm::identity<glm::mat4>()) override;
            virtual void Draw(Material::MaterialBase& material, const glm::mat4& model_matrix = glm::identity<glm::mat4>()) override;
            virtual void Draw(Renderer::Module& module, const glm::mat4& model_matrix = glm::identity<glm::mat4>()) override;
            
            virtual void DisplayWidget(std::set<std::shared_ptr<Material::MaterialBase>> materials);

            // Default material is PBRMetallicMaterial
            std::shared_ptr<Material::MaterialBase> material = std::make_shared<Material::PBRMetallicMaterial>();
            
            std::shared_ptr<Core::Vao> vao;
            
        protected:
            Mesh(ComponentType type);

    };

}