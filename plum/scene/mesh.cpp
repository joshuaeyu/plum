#include <plum/scene/mesh.hpp>

#include <plum/component/core.hpp>

#include <vector>
#include <memory>

namespace Component {

    Mesh::Mesh()
        : SceneObject(SceneObjectType::Mesh) 
    {}

    Mesh::Mesh(Vao& vao) 
        : SceneObject(SceneObjectType::Mesh), vao(std::make_shared<Vao>(vao)) 
    {}
    
    Mesh::Mesh(Vao& vao, Material::Material& mat)
        : SceneObject(SceneObjectType::Mesh), vao(std::make_shared<Vao>(vao)), material(std::make_shared<Material::Material>(mat)) 
    {}
    
    Mesh::Mesh(const SceneObjectType type) 
        : SceneObject(type) 
    {}
    
    Mesh::~Mesh() {}

    void Mesh::Draw(const glm::mat4& model_matrix) {
        if (material)   // If no material assigned, just draw the raw VAO
            material->SetUniforms(model_matrix);
        vao->Draw();
    }
    void Mesh::Draw(Material::Material& mat, const glm::mat4& model_matrix) {
        mat.SetUniforms(model_matrix);
        vao->Draw();
    }
    void Mesh::Draw(Material::Module& module, const glm::mat4& model_matrix) {
        module.GetProgram()->SetMat4("model", model_matrix);
        vao->Draw();
    }

}