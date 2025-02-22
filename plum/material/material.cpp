#include <plum/material/material.hpp>

namespace Material {
    
    PBRMetallicMaterial::PBRMetallicMaterial() {
    }

    std::shared_ptr<Shader> PBRMetallicMaterial::GetShader() {
        return shader;
    }
    
    void PBRMetallicMaterial::SetUniforms(const glm::mat4& model_matrix) {
        shader->Use();

        // Model transform
        shader->setMat4("model", model_matrix);
        
        // Textures
        int tex_idx = 0;

        if (albedoMap) {
            glActiveTexture(GL_TEXTURE0 + tex_idx);
            shader->setInt("material.texture_albedo", tex_idx++);
            albedoMap->Bind();
        }
        if (metallicMap) {
            glActiveTexture(GL_TEXTURE0 + tex_idx);
            shader->setInt("material.texture_metallic", tex_idx++);
            metallicMap->Bind();
        }
        if (roughnessMap) {
            glActiveTexture(GL_TEXTURE0 + tex_idx);
            shader->setInt("material.texture_roughness", tex_idx++);
            roughnessMap->Bind();
        }
        if (normalMap) {
            glActiveTexture(GL_TEXTURE0 + tex_idx);
            shader->setInt("material.texture_normal", tex_idx++);
            normalMap->Bind();
        }
        if (displacementMap) {
            glActiveTexture(GL_TEXTURE0 + tex_idx);
            shader->setInt("material.texture_height", tex_idx++);
            displacementMap->Bind();
        }
        if (occlusionMap) {
            glActiveTexture(GL_TEXTURE0 + tex_idx);
            shader->setInt("material.texture_occlusion", tex_idx++);
            occlusionMap->Bind();
        }
        
        shader->setInt("material.has_texture_albedo", static_cast<bool>(albedoMap));
        shader->setInt("material.has_texture_metallic", static_cast<bool>(metallicMap));
        shader->setInt("material.has_texture_roughness", static_cast<bool>(roughnessMap));
        shader->setInt("material.has_texture_normal", static_cast<bool>(normalMap));
        shader->setInt("material.has_texture_height", static_cast<bool>(displacementMap));
        shader->setInt("material.has_texture_occlusion", static_cast<bool>(occlusionMap));

        // Raw values
        shader->setVec3("material.albedo", albedo);
        shader->setFloat("material.metallic", metallic);
        shader->setFloat("material.roughness", roughness);

    }
    
    PBRSpecularMaterial::PBRSpecularMaterial() {}
    
    PhongMaterial::PhongMaterial() {}

}