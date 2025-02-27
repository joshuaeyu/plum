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

    void Mesh::Draw(const glm::mat4& model_matrix) {
        material->SetUniforms(model_matrix);
        vao->Draw();
    }
    void Mesh::Draw(Material::Material& material, const glm::mat4& model_matrix) {
        material.SetUniforms(model_matrix);
        vao->Draw();
    }
    void Mesh::Draw(Material::Module& module, const glm::mat4& model_matrix) {
        module.GetProgram()->SetMat4("model", model_matrix);
        vao->Draw();
    }



}