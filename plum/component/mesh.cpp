#include <plum/vertex.hpp>
#include <plum/component/core.hpp>
#include <plum/component/mesh.hpp>
#include <vector>
#include <memory>

namespace Component {

    Mesh::Mesh() : SceneObject(SceneObjectType::Mesh) {}

    Mesh::Mesh(std::shared_ptr<Vao> va) : vao(va), SceneObject(SceneObjectType::Mesh) {}

    Mesh::Mesh(const SceneObjectType type) : SceneObject(type) {}
    
    Mesh::~Mesh() {}

    void Mesh::Draw() {
        // needs work
        // material->shader->Use();
        // material->shader->SetUniforms(material);
        vao->Draw();
    };
    void Mesh::Draw(const glm::mat4& model_matrix) {
        // needs work
        // material->shader->Use();
        // material->shader->SetUniforms(model_matrix, material);
        vao->Draw();
    };



}