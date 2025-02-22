#pragma once

#include <string>

#include <plum/util/transform.hpp>
#include <plum/material/material.hpp>

namespace Component {

    class SceneObject {
        public: 
            virtual ~SceneObject();
            
            virtual void Draw() {};
            virtual void Draw(const glm::mat4& parent_transf) {};
            virtual void Draw(Material::Material& m) {}
            virtual void Draw(const glm::mat4& model_matrix, Material::Material& m) {}
            
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
            void AddChild(std::shared_ptr<SceneNode> node);
            void RemoveChild(std::shared_ptr<SceneNode> node);
            
            void Draw();
            void Draw(const glm::mat4& parent_transf);
            void Draw(Material::Material& m);
            void Draw(const glm::mat4& parent_transf, Material::Material& m);
            
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