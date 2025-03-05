#include <plum/component/mesh.hpp>

#include <plum/core/core.hpp>

#include <vector>
#include <memory>

namespace Component {

    Mesh::Mesh()
        : ComponentBase(ComponentType::Mesh) 
    {}

    Mesh::Mesh(Core::Vao& vao) 
        : ComponentBase(ComponentType::Mesh), vao(std::make_shared<Core::Vao>(vao)) 
    {}
    
    Mesh::Mesh(Core::Vao& vao, Material::MaterialBase& mat)
        : ComponentBase(ComponentType::Mesh), vao(std::make_shared<Core::Vao>(vao))
    {
        material.reset(&mat);
    }
    
    Mesh::Mesh(const ComponentType type) 
        : ComponentBase(type) 
    {}
    
    Mesh::~Mesh() {}

    void Mesh::Draw(const glm::mat4& model_matrix) {
        if (material)   // If no material assigned, just draw the raw VAO
            material->SetUniforms(model_matrix);
        vao->Draw();
    }
    void Mesh::Draw(Material::MaterialBase& mat, const glm::mat4& model_matrix) {
        mat.SetUniforms(model_matrix);
        vao->Draw();
    }
    void Mesh::Draw(Material::Module& module, const glm::mat4& model_matrix) {
        module.GetProgram()->SetMat4("model", model_matrix);
        vao->Draw();
    }

}