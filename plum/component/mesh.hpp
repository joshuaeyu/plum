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
            
            std::shared_ptr<Core::Vao> vao;
            std::shared_ptr<Material::MaterialBase> material = std::make_shared<Material::PBRMetallicMaterial>();

            virtual void Draw(const glm::mat4& model_matrix = glm::identity<glm::mat4>()) override;
            virtual void Draw(Material::MaterialBase& material, const glm::mat4& model_matrix = glm::identity<glm::mat4>()) override;
            virtual void Draw(Renderer::Module& module, const glm::mat4& model_matrix = glm::identity<glm::mat4>()) override;
            
            int widgetId = -1;
            virtual void DisplayWidget(std::set<std::shared_ptr<Material::MaterialBase>> materials);

        protected:
            Mesh(ComponentType type);
    };

}