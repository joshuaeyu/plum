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
            SceneNode(Component::Component& component);
            SceneNode(std::shared_ptr<Component::Component> component);
            ~SceneNode();
               
        public:
            std::shared_ptr<SceneNode> CreateChild();
            void AddChild(Component::Component& component);
            void AddChild(SceneNode& node);
            void RemoveChild(SceneNode& node);
            
            virtual void Draw(const glm::mat4& parent_transform = glm::identity<glm::mat4>());
            virtual void Draw(Material::Material& m, const glm::mat4& parent_transform = glm::identity<glm::mat4>());
            virtual void Draw(Material::Module& m, const glm::mat4& parent_transform = glm::identity<glm::mat4>());
            
            std::string name;

            Transform transform;
            
            std::shared_ptr<Component::Component> component;
            std::shared_ptr<SceneNode> parent;
            std::vector<std::shared_ptr<SceneNode>> children;

            bool bypassLighting = false;
            bool visible = true;

        private:
            
    };

}