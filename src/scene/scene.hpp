#pragma once

#include "scene/scenenode.hpp"

namespace Scene {

    class Scene : public SceneNode {
        public:
            Scene() : SceneNode("Root") {}
    };

}