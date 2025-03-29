#include <plum/component/mesh.hpp>

#include <plum/core/core.hpp>

#include <vector>
#include <memory>
#include <iostream>

namespace Component {

    Mesh::Mesh()
        : ComponentBase(ComponentType::Mesh) 
    {}

    Mesh::Mesh(std::shared_ptr<Core::Vao> vao)
        : ComponentBase(ComponentType::Mesh), 
        vao(std::move(vao)) 
    {}
    
    Mesh::Mesh(std::shared_ptr<Core::Vao> vao, std::shared_ptr<Material::MaterialBase> mat)
        : ComponentBase(ComponentType::Mesh), 
        vao(std::move(vao)),
        material(std::move(mat))
    {}
    
    Mesh::Mesh(ComponentType type) 
        : ComponentBase(type) 
    {}
    
    Mesh::~Mesh() {
        // std::cout << "destroying Mesh" << std::endl;
    }

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