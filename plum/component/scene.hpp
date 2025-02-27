#pragma once

#include <memory>

#include <plum/component/texture.hpp>

#include <plum/component/scenenode.hpp>
#include <plum/material/material.hpp>
#include <plum/material/module.hpp>

namespace Component {

    class Scene {
        public:
            SceneNode root;

            Scene();

            void Add(std::shared_ptr<SceneNode> node);
            void Remove(std::shared_ptr<SceneNode> node);

            void Draw();
            void Draw(Material::Material& material);
            void Draw(Material::Module& module);
            
    };

}