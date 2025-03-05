#pragma once

#include <plum/core/program.hpp>
#include <plum/core/tex.hpp>

#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <string>

namespace Material {
    
    class MaterialBase {
        public:
            virtual std::shared_ptr<Core::Program> GetProgram() = 0;
            virtual void SetUniforms(const glm::mat4& model_transform) = 0;
        protected:
            MaterialBase() {}
    };

    class PBRMetallicMaterial : public MaterialBase {
        public:
            PBRMetallicMaterial();
            std::shared_ptr<Core::Program> GetProgram() override;
            void SetUniforms(const glm::mat4& model_transform) override;

            std::shared_ptr<Core::Tex2D> albedoMap, metallicMap, roughnessMap, normalMap, displacementMap, occlusionMap;
            glm::vec3 albedo = glm::vec3(0.5);
            float metallic = 0.5;
            float roughness = 0.5;
        private:
            inline static const std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_gen.vs", "shaders/shaderf_basichybrid.fs");
    };

    // class PBRSpecularMaterial : public MaterialBase {
    //     public:
    //         PBRSpecularMaterial();
    //         std::shared_ptr<Core::Program> GetProgram() override;
    //         void SetUniforms(const glm::mat4& model_transform) override;

    //         std::shared_ptr<Core::Tex2D> albedoMap, specularMap, roughnessMap, normalMap, displacementMap, occlusionMap;
    //         glm::vec3 albedo = glm::vec3(0.5);
    //         glm::vec3 specular = glm::vec3(0.5);
    //         float roughness = 0.5;
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