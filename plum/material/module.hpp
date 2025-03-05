#pragma once

#include <plum/core/program.hpp>

#include <memory>

namespace Component 
{
    class DirectionalLight;
    class PointLight;
}

namespace Material
{

    class Module
    {
        public:
            virtual std::shared_ptr<Core::Program> GetProgram() = 0;
            virtual void Use() = 0;
        protected:
            Module() {}
            // std::shared_ptr<Program> program;
    };

    class DirectionalShadowModule : public Module
    {
        public:
            DirectionalShadowModule();
            void SetGlobalUniforms(Component::DirectionalLight& dl, const GLuint depthTexture, int* shadowIdx);
            void SetObjectUniforms(const glm::mat4& model);
            void Use() override;
            std::shared_ptr<Core::Program> GetProgram() override;

            inline static std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_shadow2d.vs", "shaders/shaderf_shadow2d.fs");
    };

    class PointShadowModule : public Module
    {
        public:
            PointShadowModule();
            void SetGlobalUniforms(Component::PointLight& pl, const glm::vec3& pos, int* shadowIdx);
            void SetObjectUniforms(const glm::mat4& model);
            void Use() override;
            std::shared_ptr<Core::Program> GetProgram() override;

            inline static std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_shadowcube.vs", "shaders/shaderf_shadowcube.fs", "shaders/shaderg_shadowcube.gs");
    };

    class LightingPassPBRModule : public Module
    {
        public:
            LightingPassPBRModule();
            void SetGlobalUniforms();
            void Use() override;
            std::shared_ptr<Core::Program> GetProgram() override;

            int gPosition, gNormal, gAlbedoSpec, gMetRouOcc;
            int irradianceMap, prefilterMap, brdfLUT;
            float ibl;
            int shadowmap_2d_array_shadow, shadowmap_cube_array_shadow;

            inline static std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_lightingpasspbr.fs");
    };

    class SkyboxModule : public Module 
    {
        public:
            SkyboxModule();
            void SetGlobalUniforms(const glm::mat4& view, const glm::mat4& projection, const int cubemapUnit);
            void Use() override;
            std::shared_ptr<Core::Program> GetProgram() override;

            bool hdr = true;

            inline static const std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_skybox.vs", "shaders/shaderf_skybox.fs");
            inline static const std::shared_ptr<Core::Program> hdrProgram = std::make_shared<Core::Program>("shaders/shaderv_skybox.vs", "shaders/shaderf_skyboxhdr.fs");
    };

}