#pragma once

#include <vector>
#include <memory>

#include <plum/component/core.hpp>
#include <plum/component/scenenode.hpp>
#include <plum/material/material.hpp>

#include <plum/shader.hpp>

namespace Component {

    class Mesh : public SceneObject {
        // SceneNode has Draw(), transform, parent, children
        
        public:
            Mesh();
            Mesh(std::shared_ptr<Vao> va);
            virtual ~Mesh();
            
            virtual void Draw() override;
            virtual void Draw(const glm::mat4& parent_transf) override;
            
            std::shared_ptr<Material::Material> material;
            // friend class Renderer;
            
            std::shared_ptr<Vao> vao;
            
        protected:
            Mesh(const SceneObjectType type);

    };

}