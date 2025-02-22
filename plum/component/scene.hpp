#pragma once

#include <memory>

#include <plum/component/texture.hpp>

#include <plum/component/scenenode.hpp>

namespace Component {

    class Scene {
        public:
            SceneNode root;
            std::shared_ptr<Texture> skybox;

            Scene();

            void Add(std::shared_ptr<SceneNode> node);
            void Remove(std::shared_ptr<SceneNode> node);

            void Draw();
            
    };

}