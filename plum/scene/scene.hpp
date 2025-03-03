#pragma once


#include <plum/scene/scenenode.hpp>

#include <plum/material/material.hpp>
#include <plum/material/module.hpp>

#include <memory>

namespace Component {

    typedef SceneNode Scene;

    // class Scene {
    //     public:
    //         SceneNode root;

    //         Scene();

    //         void Add(std::shared_ptr<SceneNode> node);
    //         void Remove(std::shared_ptr<SceneNode> node);

    //         void Draw();
    //         void Draw(Material::Material& material);
    //         void Draw(Material::Module& module);
            
    // };

}