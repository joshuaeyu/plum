#pragma once

#include <plum/core/program.hpp>
#include <plum/core/core.hpp>

#include <memory>

namespace Component 
{
    class DirectionalLight;
    class PointLight;
}

namespace Scene
{
    class SceneNode;
    typedef SceneNode Scene;
}

namespace Material
{

    class Module
    {
        public:
            virtual std::shared_ptr<Core::Program> GetProgram() = 0;
        protected:
            Module() {}
            // std::shared_ptr<Program> program;
    };

    class DirectionalShadowModule : public Module
    {
        public:
            DirectionalShadowModule(int map_width = 512, int map_height = 512, int num_layers = 8);

            void Render(Scene::Scene& scene, const std::vector<Scene::SceneNode*>& dirlight_nodes);

            void SetObjectUniforms(const glm::mat4& model);

            std::shared_ptr<Core::Program> GetProgram() override;

            
            const int mapWidth, mapHeight, numLayers;
            const std::shared_ptr<Core::Tex3D> depthMap;
            
        private:
            Core::Fbo fbo;

            void setGlobalUniforms(Component::DirectionalLight& dl, const GLuint depth_texture, int* shadow_idx);
            
            inline static std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_shadow2d.vs", "shaders/shaderf_shadow2d.fs");
            
        };
        
    class PointShadowModule : public Module
    {
        public:
            PointShadowModule(int map_width = 512, int map_height = 512, int num_layers = 8);

            void Render(Scene::Scene& scene, const std::vector<Scene::SceneNode*>& pointlight_nodes);
            
            void SetObjectUniforms(const glm::mat4& model);
            
            std::shared_ptr<Core::Program> GetProgram() override;
            
            const int mapWidth, mapHeight, numLayers;
            const std::shared_ptr<Core::Tex3D> depthMap;
            
        private:
            Core::Fbo fbo;
            
            void setGlobalUniforms(Component::PointLight& pl, const glm::vec3& position, int* shadow_idx);

            inline static std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_shadowcube.vs", "shaders/shaderf_shadowcube.fs", "shaders/shaderg_shadowcube.gs");
        };

    class LightingPassPBRModule : public Module
    {
        public:
            LightingPassPBRModule();
            void SetGlobalUniforms();

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

            std::shared_ptr<Core::Program> GetProgram() override;

            bool hdr = true;

            inline static const std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_skybox.vs", "shaders/shaderf_skybox.fs");
            inline static const std::shared_ptr<Core::Program> hdrProgram = std::make_shared<Core::Program>("shaders/shaderv_skybox.vs", "shaders/shaderf_skyboxhdr.fs");
    };

}