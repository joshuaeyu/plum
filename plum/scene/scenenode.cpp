#include <plum/scene/scenenode.hpp>

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <string>
#include <iostream>

namespace Scene {
    
    SceneNode::SceneNode() {}
    SceneNode::SceneNode(std::shared_ptr<Component::ComponentBase> component) 
        : component(component) 
    {}
    SceneNode::~SceneNode() {
        // std::cout << "destroying SceneNode" << std::endl;
    }

    // Methods
    void SceneNode::Draw(const glm::mat4& parent_transform) {
        glm::mat4 model_matrix = transform.Matrix() * parent_transform;
        if (component)
            component->Draw(model_matrix);
        for (auto& child : children) {
            child->Draw(model_matrix);
        }
    }
    void SceneNode::Draw(Material::MaterialBase& material, const glm::mat4& parent_transform) {
        glm::mat4 model_matrix = transform.Matrix() * parent_transform;
        if (component)
            component->Draw(material, model_matrix);
        for (auto& child : children) {
            child->Draw(material, model_matrix);
        }
    }
    void SceneNode::Draw(Material::Module& module, const glm::mat4& parent_transform) {
        glm::mat4 model_matrix = transform.Matrix() * parent_transform;
        if (component)
            component->Draw(module, model_matrix);
        for (auto& child : children) {
            child->Draw(module, model_matrix);
        }
    }

    // Modifiers
    std::shared_ptr<SceneNode> SceneNode::CreateChild() {
        children.push_back(std::make_shared<SceneNode>());
        return children.back();
    }
    void SceneNode::AddChild(std::shared_ptr<SceneNode> node) {
        children.push_back(node);
    }
    void SceneNode::AddChild(std::shared_ptr<Component::ComponentBase> component) {
        children.push_back(std::make_shared<SceneNode>(component));
    }
    void SceneNode::RemoveChild(std::shared_ptr<SceneNode> node) {
        auto it = std::find(children.begin(), children.end(), node);
        children.erase(it);
    }

}