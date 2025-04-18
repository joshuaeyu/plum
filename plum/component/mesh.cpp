#include <plum/component/mesh.hpp>

#include <plum/core/globject.hpp>
#include <plum/interface/widget.hpp>

#include <imgui/imgui.h>

#include <vector>
#include <memory>
#include <iostream>

namespace Component {

    Mesh::Mesh()
        : ComponentBase(ComponentType::Mesh) 
    {
        name = "Mesh";
    }

    Mesh::Mesh(std::shared_ptr<Core::Vao> vao)
        : ComponentBase(ComponentType::Mesh),
        vao(std::move(vao)) 
    {
        name = "Mesh";
    }
    
    Mesh::Mesh(std::shared_ptr<Core::Vao> vao, std::shared_ptr<Material::MaterialBase> mat)
        : ComponentBase(ComponentType::Mesh), 
        vao(std::move(vao)),
        material(std::move(mat))
    {
        name = "Mesh";
    }
    
    Mesh::Mesh(ComponentType type) 
        : ComponentBase(type) 
    {
        name = "Mesh";
    }
    
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
    void Mesh::Draw(Renderer::Module& module, const glm::mat4& model_matrix) {
        module.GetProgram()->SetMat4("model", model_matrix);
        vao->Draw();
    }

    void Mesh::DisplayWidget(std::map<std::string, std::shared_ptr<Material::MaterialBase>> materials) {
        static int itemSelectedIdx = 0;
        std::vector<char*> itemNames(materials.size());
        for (int i = 0; i < materials.size(); i++) {
            itemNames[i] = new char[256];
            strcpy(itemNames[i], (std::next(materials.begin(), i)->first).c_str());
        }
        ImGui::Combo("Material", &itemSelectedIdx, itemNames.data(), itemNames.size());
        for (int i = 0; i < materials.size(); i++) {
            delete itemNames[i];
        }
        material = std::next(materials.begin(), itemSelectedIdx)->second;
    }

}