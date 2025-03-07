#pragma once

#include <string>

#include <plum/component/component.hpp>
#include <plum/material/material.hpp>
#include <plum/material/module.hpp>
#include <plum/util/transform.hpp>

namespace Scene {

// each scene node type will implement its own GUI module?

    class SceneNode {
        public:
            SceneNode();
            SceneNode(std::shared_ptr<Component::ComponentBase> component);
            ~SceneNode();
               
        public:
            std::shared_ptr<SceneNode> CreateChild();
            void AddChild(std::shared_ptr<Component::ComponentBase> component);
            void AddChild(std::shared_ptr<SceneNode> node);
            void RemoveChild(std::shared_ptr<SceneNode> node);
            
            virtual void Draw(const glm::mat4& parent_transform = glm::identity<glm::mat4>());
            virtual void Draw(Material::MaterialBase& m, const glm::mat4& parent_transform = glm::identity<glm::mat4>());
            virtual void Draw(Material::Module& m, const glm::mat4& parent_transform = glm::identity<glm::mat4>());
            
            std::string name;

            Transform transform;
            
            std::shared_ptr<Component::ComponentBase> component;
            std::shared_ptr<SceneNode> parent;
            std::vector<std::shared_ptr<SceneNode>> children;

            bool bypassLighting = false;
            bool visible = true;
            
    };

}