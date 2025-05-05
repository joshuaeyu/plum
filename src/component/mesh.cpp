#include "component/mesh.hpp"

#include "core/globject.hpp"
#include "interface/widget.hpp"

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
        // std::clog << "destroying Mesh" << std::endl;
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
    void Mesh::Draw(Renderer::RenderModule& module, const glm::mat4& model_matrix) {
        if (module.AllowDraw(*this)) {
            module.SetObjectUniforms(model_matrix, *this);
            vao->Draw();
        }
    }

    void Mesh::DisplayWidget() {
        int itemSelectedIdx = 0;
        std::vector<char*> itemNames(Material::materials.size());

        // Determine index of current material
        for (int i = 0; i < Material::materials.size(); i++) {
            itemNames[i] = new char[256];
            strcpy(itemNames[i], (*std::next(Material::materials.begin(), i))->name.c_str());
            if (strcmp(material->name.c_str(), itemNames[i]) == 0) {
                itemSelectedIdx = i;
            }
        }

        // Update material if index has changed
        int idxBefore = itemSelectedIdx;
        ImGui::Combo("Material", &itemSelectedIdx, itemNames.data(), itemNames.size());
        if (idxBefore != itemSelectedIdx) {
            material = *std::next(Material::materials.begin(), itemSelectedIdx);
        }

        for (int i = 0; i < itemNames.size(); i++) {
            delete itemNames[i];
        }
    }

}