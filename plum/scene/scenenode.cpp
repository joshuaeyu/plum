#include <plum/scene/scenenode.hpp>

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <string>
#include <iostream>

namespace Scene {
    
    SceneNode::SceneNode() {}
    SceneNode::SceneNode(std::shared_ptr<Component::ComponentBase> component) 
        : component(std::move(component))
    {
        if (!this->component->name.empty()) {
            name = this->component->name;
        }
    }
    SceneNode::~SceneNode() {
        // std::cout << "destroying SceneNode" << std::endl;
    }

    // Methods
    void SceneNode::Draw(const glm::mat4& parent_transform) {
        glm::mat4 model_matrix = parent_transform * transform.Matrix();
        if (component)
            component->Draw(model_matrix);
        for (auto& child : children) {
            child->Draw(model_matrix);
        }
    }
    void SceneNode::Draw(Material::MaterialBase& material, const glm::mat4& parent_transform) {
        glm::mat4 model_matrix = parent_transform * transform.Matrix();
        if (component)
            component->Draw(material, model_matrix);
        for (auto& child : children) {
            child->Draw(material, model_matrix);
        }
    }
    void SceneNode::Draw(Renderer::Module& module, const glm::mat4& parent_transform) {
        glm::mat4 model_matrix = parent_transform * transform.Matrix();
        if (component)
            component->Draw(module, model_matrix);
        for (auto& child : children) {
            child->Draw(module, model_matrix);
        }
    }

    // Modifiers
    std::shared_ptr<SceneNode> SceneNode::CreateChild() {
        children.emplace_back(std::make_shared<SceneNode>());
        return children.back();
    }
    std::shared_ptr<SceneNode> SceneNode::AddChild(std::shared_ptr<SceneNode> node) {
        children.emplace_back(std::move(node));
        return children.back();
    }
    std::shared_ptr<SceneNode> SceneNode::AddChild(std::shared_ptr<Component::ComponentBase> component) {
        children.emplace_back(std::make_shared<SceneNode>(component));
        return children.back();
    }
    void SceneNode::RemoveChild(std::shared_ptr<SceneNode> node) {
        auto it = std::find(children.begin(), children.end(), node);
        children.erase(it);
    }

}