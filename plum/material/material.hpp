#pragma once

#include <vector>
#include <memory>
#include <string>

#include <glm/glm.hpp>

#include <plum/component/tex.hpp>
#include <plum/material/program.hpp>

namespace Material {
    
    class Material : public ProgramUser {
        public:
            virtual std::shared_ptr<Program> GetProgram() = 0;
            virtual void SetUniforms(const glm::mat4& model_transform) = 0;
        protected:
            Material();
    };

    class PBRMetallicMaterial : public Material {
        public:
            PBRMetallicMaterial();
            std::shared_ptr<Program> GetProgram() override;
            void SetUniforms(const glm::mat4& model_transform) override;

            std::shared_ptr<Component::Tex2D> albedoMap, metallicMap, roughnessMap, normalMap, displacementMap, occlusionMap;
            glm::vec3 albedo = glm::vec3(0.5);
            float metallic = 0.5;
            float roughness = 0.5;
        private:
            inline static const std::shared_ptr<Program> program = std::make_shared<Program>("shaders/shaderv_gen.vs, shaders/shaderf_basichybrid.fs");
    };

    class PBRSpecularMaterial : public Material {
        public:
            PBRSpecularMaterial();
            std::shared_ptr<Program> GetProgram() override;
            void SetUniforms(const glm::mat4& model_transform) override;

            std::shared_ptr<Component::Tex2D> albedoMap, specularMap, roughnessMap, normalMap, displacementMap, occlusionMap;
            glm::vec3 albedo = glm::vec3(0.5);
            glm::vec3 specular = glm::vec3(0.5);
            float roughness = 0.5;
        private:

    };

    class PhongMaterial : public Material {
        public:
            PhongMaterial();
            std::shared_ptr<Program> GetProgram() override;
            void SetUniforms(const glm::mat4& model_transform) override;

            glm::vec3 ambient = glm::vec3(0.5);
            glm::vec3 diffuse = glm::vec3(0.5);
            glm::vec3 specular = glm::vec3(0.5);
    };

}