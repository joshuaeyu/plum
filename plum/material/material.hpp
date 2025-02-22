#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include <plum/texture.hpp>
#include <plum/material/shader.hpp>

namespace Material {
    
    class Material {
        public:
            std::shared_ptr<Shader> shader;
        protected:
            Material();
    };

    class PBRMetallicMaterial : public Material {
        public:
            PBRMetallicMaterial();
            std::shared_ptr<Tex> albedoMap, metallicMap, roughnessMap, normalMap, displacementMap, occlusionMap;
            glm::vec3 albedo = glm::vec3(0.5);
            float metallic = 0.5;
            float roughness = 0.5;
        private:

    };

    class PBRSpecularMaterial : public Material {
        public:
            PBRSpecularMaterial();
            std::shared_ptr<Tex> albedoMap, specularMap, roughnessMap, normalMap, displacementMap, occlusionMap;
            glm::vec3 albedo = glm::vec3(0.5);
            glm::vec3 specular = glm::vec3(0.5);
            float roughness = 0.5;
        private:

    };

    class PhongMaterial : public Material {
        public:
            PhongMaterial();
            glm::vec3 ambient = glm::vec3(0.5);
            glm::vec3 diffuse = glm::vec3(0.5);
            glm::vec3 specular = glm::vec3(0.5);
    };

    // compatibility between shader and material
    // PBR vs basic shading
    // SHADERS AND MATERIALS ARE TIGHTLY COUPLED
    // shader.setuniforms(material) overload for material type check!
    // shader.resetuniforms(material)   overload for material type check!

}