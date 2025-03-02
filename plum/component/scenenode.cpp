#include <plum/component/scenenode.hpp>

#include <string>

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <plum/shader.hpp>

namespace Component {

    SceneObject::SceneObject(const SceneObjectType type) : objType(type) {}
    SceneObject::~SceneObject() {}

    const bool SceneObject::IsMesh() const {
        return objType == SceneObjectType::Mesh
                || objType == SceneObjectType::Model
                || objType == SceneObjectType::Primitive;
    }
    const bool SceneObject::IsLight() const {
        return objType == SceneObjectType::DirLight
                || objType == SceneObjectType::PointLight;
    }
    
    SceneNode::SceneNode() {}
    SceneNode::SceneNode(SceneObject& object) : object(std::make_shared<SceneObject>(object)) {}
    SceneNode::SceneNode(std::shared_ptr<SceneObject> object) : object(object) {}

    // Methods
    void SceneNode::Draw(const glm::mat4& parent_transform) {
        glm::mat4 model_matrix = transform.Matrix() * parent_transform;
        object->Draw(model_matrix);
        for (auto& child : children) {
            child->Draw(model_matrix);
        }
    }
    void SceneNode::Draw(Material::Material& material, const glm::mat4& parent_transform) {
        glm::mat4 model_matrix = transform.Matrix() * parent_transform;
        object->Draw(material, model_matrix);
        for (auto& child : children) {
            child->Draw(material, model_matrix);
        }
    }
    void SceneNode::Draw(Material::Module& module, const glm::mat4& parent_transform) {
        glm::mat4 model_matrix = transform.Matrix() * parent_transform;
        object->Draw(module, model_matrix);
        for (auto& child : children) {
            child->Draw(module, model_matrix);
        }
    }

    // Modifiers
    std::shared_ptr<SceneNode> SceneNode::CreateChild() {
        children.emplace_back(SceneNode());
        return children.back();
    }
    void SceneNode::AddChild(SceneNode& node) {
        children.emplace_back(node);
    }
    void SceneNode::AddChild(SceneObject& object) {
        children.emplace_back(object);
    }
    void SceneNode::RemoveChild(SceneNode& node) {
        auto it = std::find(children.begin(), children.end(), node);
        children.erase(it);
    }

}