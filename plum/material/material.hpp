#pragma once

#include <plum/core/program.hpp>
#include <plum/core/tex.hpp>

#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <string>

namespace Component {
    struct MaterialInfo;
}

namespace Material {
    
    class MaterialBase {
        public:
            virtual ~MaterialBase();
            virtual std::shared_ptr<Core::Program> GetProgram() { return nullptr; }
            virtual void SetUniforms(const glm::mat4& model_transform) {};
            virtual void DisplayWidget() {};
        protected:
            MaterialBase() = default;
    };

    class PBRMetallicMaterial : public MaterialBase {
        public:
            PBRMetallicMaterial();
            PBRMetallicMaterial(Component::MaterialInfo info);

            void processMaterialInfo(Component::MaterialInfo info);

            std::shared_ptr<Core::Program> GetProgram() override;
            void SetUniforms(const glm::mat4& model_transform) override;

            void DisplayWidget() override;

            std::shared_ptr<Core::Tex2D> albedoMap, metallicMap, roughnessMap, normalMap, displacementMap, occlusionMap;
            glm::vec3 albedo = glm::vec3(0.5);
            float metallic = 0.5;
            float roughness = 0.5;
            
        private:
            inline static std::shared_ptr<Core::Program> program;
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