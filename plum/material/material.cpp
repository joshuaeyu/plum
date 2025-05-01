#include <plum/material/material.hpp>

#include <plum/asset/manager.hpp>
#include <plum/asset/shader.hpp>
#include <plum/component/model.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

#include <iostream>
#include <memory>

namespace Material {
    
    MaterialBase::~MaterialBase() {
        // std::clog << "destroying MaterialBase" << std::endl;
    }

    PBRMetallicMaterial::PBRMetallicMaterial()
    {
        name = "PBR Metallic";
        // Set UBO scheme to default (may implement non-default schemes in the future)
        if (!program) {
            AssetManager& manager = AssetManager::Instance();

            auto vs = manager.LoadHot<ShaderAsset>("shaders/shaderv_gen.vs", GL_VERTEX_SHADER);
            auto fs = manager.LoadHot<ShaderAsset>("shaders/shaderf_basichybrid.fs", GL_FRAGMENT_SHADER);
            
            program = std::make_shared<Core::Program>(vs, fs);
            program->SetUniformBlockBindingScheme(Core::Program::UboScheme::Scheme1);
        }
    }

    PBRMetallicMaterial::PBRMetallicMaterial(Component::MaterialInfo info) 
        : PBRMetallicMaterial()
    {
        processMaterialInfo(info);
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
        static const Directory textureDirectory("assets/textures");
        
        nameWidget.Display(&name);

        if (albedoWidget.Display(textureDirectory, "Albedo Map", AssetUtils::imageExtensions, &albedoPath, Path())) {
            if (!albedoPath.IsEmpty()) {
                auto image = AssetManager::Instance().LoadHot<ImageAsset>(albedoPath);
                albedoMap = std::make_shared<Texture>(image, Material::TextureType::Diffuse)->tex;
            } else {
                albedoMap.reset();
            }
        }
        
        if (normalWidget.Display(textureDirectory, "Normal Map", AssetUtils::imageExtensions, &normalPath, Path())) {
            if (!normalPath.IsEmpty()) {
                auto image = AssetManager::Instance().LoadHot<ImageAsset>(normalPath);
                normalMap = std::make_shared<Texture>(image, Material::TextureType::Normal)->tex;
            } else {
                normalMap.reset();
            }
        }
        
        ImGui::ColorEdit3("Albedo", glm::value_ptr(albedo), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
        ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f, "%.3f");
    }

    void PBRMetallicMaterial::processMaterialInfo(Component::MaterialInfo info) {
        albedo = info.diffuse;
        metallic = info.metalness;
        roughness = info.roughness;
        for (auto& texture : info.textures) {
            if (texture->type == Material::TextureType::Diffuse) {
                albedoMap = texture->tex;
                albedo = glm::vec3(0);
            }
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