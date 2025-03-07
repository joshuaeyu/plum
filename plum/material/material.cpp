#include <plum/material/material.hpp>

#include <iostream>

namespace Material {
    
    MaterialBase::~MaterialBase() {
        // std::cout << "destroying MaterialBase" << std::endl;
    }

    PBRMetallicMaterial::PBRMetallicMaterial() 
    {
        // Set UBO scheme to default (may implement non-default schemes in the future)
        program->SetUniformBlockBindingScheme(Core::Program::UboScheme::Scheme1);
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
    
    // PBRSpecularMaterial::PBRSpecularMaterial() {}
    
    // PhongMaterial::PhongMaterial() {}

}