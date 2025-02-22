#include <plum/component/scene.hpp>

namespace Component {

    Scene::Scene() {}

    void Scene::Add(std::shared_ptr<SceneNode> node) {
        root.AddChild(node);
    }
    void Scene::Remove(std::shared_ptr<SceneNode> node) {
        root.RemoveChild(node);
    }

    void Scene::Draw() {
        root.Draw();
    }

}