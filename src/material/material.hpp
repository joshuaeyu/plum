#pragma once

#include "core/program.hpp"
#include "core/tex.hpp"
#include "interface/widget.hpp"

#include <glm/glm.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Component {
    struct MaterialInfo;
}

namespace Material {

    inline std::set<std::shared_ptr<Material::MaterialBase>> materials;
    
    class MaterialBase {
        public:
            virtual ~MaterialBase();
            
            std::string name;
            
            virtual std::shared_ptr<Core::Program> GetProgram() = 0;
            virtual void SetUniforms(const glm::mat4& model_transform) = 0;
            virtual void DisplayWidget() {}
        
        protected:
            MaterialBase() = default;
    };

    class PBRMetallicMaterial : public MaterialBase {
        public:
            PBRMetallicMaterial();
            PBRMetallicMaterial(Component::MaterialInfo info);

            std::shared_ptr<Core::Tex2D> albedoMap, metallicMap, roughnessMap, normalMap, displacementMap, occlusionMap;
            glm::vec3 albedo = glm::vec3(0.5f);
            float metallic = 0.0f;
            float roughness = 0.5f;

            std::shared_ptr<Core::Program> GetProgram() override { return program; }
            void SetUniforms(const glm::mat4& model_transform) override;
            
            Path albedoPath, metallicPath, roughnessPath, normalPath, displacementPath, occlusionPath;
            Interface::PathComboWidget albedoWidget, metallicWidget, roughnessWidget, normalWidget, displacementWidget, occlusionWidget;
            Interface::TextEditWidget nameWidget;
            void DisplayWidget() override;
            
        private:
            inline static std::shared_ptr<Core::Program> program;
            
            void processMaterialInfo(Component::MaterialInfo info);
    };

    // class PBRSpecularMaterial : public MaterialBase {
    //     public:
    //         PBRSpecularMaterial();
    //         std::shared_ptr<Core::Program> GetProgram() override;
    //         void SetUniforms(const glm::mat4& model_transform) override;

    //         std::shared_ptr<Core::Tex2D> albedoMap, specularMap, roughnessMap, normalMap, displacementMap, occlusionMap;
    //         glm::vec3 albedo = glm::vec3(0.5);
    //         glm::vec3 specular = glm::vec3(0.5);
    //         float glossiness = 0.5;
    //     private:

    // };

    // class PhongMaterial : public MaterialBase {
    //     public:
    //         PhongMaterial();
    //         std::shared_ptr<Core::Program> GetProgram() override;
    //         void SetUniforms(const glm::mat4& model_transform) override;

    //         glm::vec3 ambient = glm::vec3(0.5);
    //         glm::vec3 diffuse = glm::vec3(0.5);
    //         glm::vec3 specular = glm::vec3(0.5);
    // };

}