#pragma once

#include <plum/scene/scenenode.hpp>

namespace Scene {

    class Scene : public SceneNode {
        public:
            Scene() : SceneNode("Root") {}
    };

}