#pragma once

#include <string>

#include <plum/component/component.hpp>
#include <plum/material/material.hpp>
#include <plum/renderer/module.hpp>
#include <plum/util/transform.hpp>

namespace Scene {

// each scene node type will implement its own GUI module?

    class SceneNode {
        public:
            SceneNode();
            SceneNode(const std::string& name);
            SceneNode(std::shared_ptr<Component::ComponentBase> component);
            SceneNode(std::shared_ptr<Component::ComponentBase> component, const std::string& name);
            ~SceneNode();

            int widgetId = -1;
            bool editingName = false;
            bool DisplayWidget(std::set<std::shared_ptr<Material::MaterialBase>> materials);
               
        public:
            std::shared_ptr<SceneNode> AddChild(std::shared_ptr<SceneNode> node);
            template<typename... Args>
            std::shared_ptr<SceneNode> EmplaceChild(Args&& ...args) {
                children.emplace_back(std::make_shared<SceneNode>(args...));
                return children.back();
            }
            void RemoveChild(std::shared_ptr<SceneNode> node);
            
            void Draw(const glm::mat4& parent_transform = glm::identity<glm::mat4>());
            void Draw(Material::MaterialBase& m, const glm::mat4& parent_transform = glm::identity<glm::mat4>());
            void Draw(Renderer::Module& m, const glm::mat4& parent_transform = glm::identity<glm::mat4>());
            
            std::string name = "SceneNode";

            Transform transform;
            
            std::shared_ptr<Component::ComponentBase> component;
            std::shared_ptr<SceneNode> parent;
            std::vector<std::shared_ptr<SceneNode>> children;

            bool bypassLighting = false;
            bool visible = true;
    };

}