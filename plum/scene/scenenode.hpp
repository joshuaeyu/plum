#pragma once

#include <string>

#include <plum/material/material.hpp>
#include <plum/material/module.hpp>
#include <plum/util/transform.hpp>

namespace Component {

    class SceneObject {
        public: 
            virtual ~SceneObject();
            
            virtual void Draw(const glm::mat4& model_matrix = glm::identity<glm::mat4>()) {}
            virtual void Draw(Material::Material& material, const glm::mat4& model_matrix = glm::identity<glm::mat4>()) {}
            virtual void Draw(Material::Module& module, const glm::mat4& model_matrix = glm::identity<glm::mat4>()) {}
            
            enum class SceneObjectType {
                // Meshes
                Mesh,
                Model,
                Primitive,
                // Lights
                DirLight,
                PointLight
            };
            
            const SceneObjectType objType;
            
            const bool IsMesh() const;
            const bool IsLight() const;
        
        protected:
            SceneObject(const SceneObjectType type);
    };

// each scene node type will implement its own GUI module?

    class SceneNode {
        public:
            SceneNode();
            SceneNode(SceneObject& object);
            SceneNode(std::shared_ptr<SceneObject> object);
            ~SceneNode();
               
        public:
            std::shared_ptr<SceneNode> CreateChild();
            void AddChild(SceneObject& object);
            void AddChild(SceneNode& node);
            void RemoveChild(SceneNode& node);
            
            virtual void Draw(const glm::mat4& parent_transform = glm::identity<glm::mat4>());
            virtual void Draw(Material::Material& m, const glm::mat4& parent_transform = glm::identity<glm::mat4>());
            virtual void Draw(Material::Module& m, const glm::mat4& parent_transform = glm::identity<glm::mat4>());
            
            std::string name;

            Transform transform;
            
            std::shared_ptr<SceneObject> object;
            std::shared_ptr<SceneNode> parent;
            std::vector<std::shared_ptr<SceneNode>> children;

            bool bypassLighting = false;
            bool visible = true;

        private:
            
    };

}