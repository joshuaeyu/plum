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
    void SceneNode::AddChild(std::shared_ptr<SceneNode> node) {
        children.push_back(node);
    }

    void SceneNode::RemoveChild(std::shared_ptr<SceneNode> node) {
        auto it = std::find(children.begin(), children.end(), node);
        children.erase(it);
    }

}