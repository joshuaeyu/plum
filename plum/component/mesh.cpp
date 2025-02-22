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
        Draw(glm::identity<glm::mat4>());
    }
    void Mesh::Draw(const glm::mat4& model_matrix) {
        material->SetUniforms(model_matrix);
        vao->Draw();
    }
    void Mesh::Draw(Material::Material& m) {
        Draw(glm::identity<glm::mat4>(), m);
    }
    void Mesh::Draw(const glm::mat4& model_matrix, Material::Material& m) {
        m.SetUniforms(model_matrix);
        vao->Draw();
    }



}