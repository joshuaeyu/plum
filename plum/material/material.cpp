#include <plum/material/material.hpp>

#include <plum/component/model.hpp>
#include <plum/context/asset.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

#include <iostream>

namespace Material {
    
    MaterialBase::~MaterialBase() {
        // std::cout << "destroying MaterialBase" << std::endl;
    }

    PBRMetallicMaterial::PBRMetallicMaterial()
    {
        // Set UBO scheme to default (may implement non-default schemes in the future)
        if (!program) {
            Asset::AssetManager& manager = Asset::AssetManager::Instance();
            const std::vector<Path> shaderPaths = {
                "shaders/shaderv_gen.vs", 
                "shaders/shaderf_basichybrid.fs"
            };
            program = manager.ImportAsset<Core::Program>(shaderPaths, true, shaderPaths[0], shaderPaths[1]);
            program->SetUniformBlockBindingScheme(Core::Program::UboScheme::Scheme1);
        }
    }

    PBRMetallicMaterial::PBRMetallicMaterial(Component::MaterialInfo info) 
        : PBRMetallicMaterial()
    {
        processMaterialInfo(info);
    }

    std::shared_ptr<Core::Program> PBRMetallicMaterial::GetProgram() {
        return program;
    }
    
    void PBRMetallicMaterial::SetUniforms(const glm::mat4& model_matrix) {
        program->Use();

        // Model transform
        program->SetMat4("model", model_matrix);
        
        // Textures
        int tex_idx = 0;

        if (albedoMap) {
            albedoMap->Bind(tex_idx);
            program->SetInt("material.texture_albedo", tex_idx++);
        }
        if (metallicMap) {
            metallicMap->Bind(tex_idx);
            program->SetInt("material.texture_metallic", tex_idx++);
        }
        if (roughnessMap) {
            roughnessMap->Bind(tex_idx);
            program->SetInt("material.texture_roughness", tex_idx++);
        }
        if (normalMap) {
            normalMap->Bind(tex_idx);
            program->SetInt("material.texture_normal", tex_idx++);
        }
        if (displacementMap) {
            displacementMap->Bind(tex_idx);
            program->SetInt("material.texture_height", tex_idx++);
        }
        if (occlusionMap) {
            occlusionMap->Bind(tex_idx);
            program->SetInt("material.texture_occlusion", tex_idx++);
        }
        
        program->SetInt("material.has_texture_albedo", static_cast<bool>(albedoMap));
        program->SetInt("material.has_texture_metallic", static_cast<bool>(metallicMap));
        program->SetInt("material.has_texture_roughness", static_cast<bool>(roughnessMap));
        program->SetInt("material.has_texture_normal", static_cast<bool>(normalMap));
        program->SetInt("material.has_texture_height", static_cast<bool>(displacementMap));
        program->SetInt("material.has_texture_occlusion", static_cast<bool>(occlusionMap));

        // Raw values
        program->SetVec3("material.albedo", albedo);
        program->SetFloat("material.metallic", metallic);
        program->SetFloat("material.roughness", roughness);

    }
    
    void PBRMetallicMaterial::DisplayWidget() {
        ImGui::ColorEdit3("Albedo", glm::value_ptr(albedo), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
        ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f, "%.3f");
    }

    void PBRMetallicMaterial::processMaterialInfo(Component::MaterialInfo info) {
        albedo = info.diffuse;
        metallic = info.metalness;
        roughness = info.roughness;
        for (auto& texture : info.textures) {
            if (texture->type == Material::TextureType::Diffuse)
                albedoMap = texture->tex;
            else if (texture->type == Material::TextureType::Metalness)
                metallicMap = texture->tex;
            else if (texture->type == Material::TextureType::Roughness)
                roughnessMap = texture->tex;
            else if (texture->type == Material::TextureType::Normal)
                normalMap = texture->tex;
            else if (texture->type == Material::TextureType::Height)
                displacementMap = texture->tex;
            else if (texture->type == Material::TextureType::Occlusion)
                occlusionMap = texture->tex;
        }
    }

    // PBRSpecularMaterial::PBRSpecularMaterial() {}
    
    // PhongMaterial::PhongMaterial() {}

}