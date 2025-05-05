#pragma once

#include "component/component.hpp"

#include "core/globject.hpp"
#include "material/material.hpp"
#include "renderer/module.hpp"
#include "scene/scenenode.hpp"

#include <memory>

namespace Component {

    class Mesh : public ComponentBase {
        public:
            Mesh();
            Mesh(std::shared_ptr<Core::Vao> vao);
            Mesh(std::shared_ptr<Core::Vao> vao, std::shared_ptr<Material::MaterialBase> material);
            virtual ~Mesh();

            std::shared_ptr<Core::Vao> vao;
            std::shared_ptr<Material::MaterialBase> material = Material::defaultMaterial;

            virtual void Draw(const glm::mat4& model_matrix = glm::identity<glm::mat4>()) override;
            virtual void Draw(Material::MaterialBase& material, const glm::mat4& model_matrix = glm::identity<glm::mat4>()) override;
            virtual void Draw(Renderer::RenderModule& module, const glm::mat4& model_matrix = glm::identity<glm::mat4>()) override;

            std::shared_ptr<ComponentBase> Duplicate() override { return nullptr; } // future
            
            // GUI widget
            int widgetId = -1;
            void DisplayWidget() override;

        protected:
            Mesh(ComponentType type);
    };

}