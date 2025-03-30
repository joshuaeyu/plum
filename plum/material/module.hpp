#pragma once

#include <plum/core/program.hpp>
#include <plum/core/globject.hpp>

#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace Component  {
    class DirectionalLight;
    class PointLight;
    class Camera;
}

namespace Scene {
    class SceneNode;
    typedef SceneNode Scene;
}

namespace Material {

    class Module {
        public:
            virtual std::shared_ptr<Core::Program> GetProgram() = 0;
        protected:
            Module() = default;
            virtual ~Module() = default;
            // std::shared_ptr<Program> program;
    };

    class DirectionalShadowModule : public Module {
        public:
            DirectionalShadowModule(int map_width = 512, int map_height = 512, int num_layers = 8);

            void Render(Scene::Scene& scene, const std::vector<Scene::SceneNode*>& dirlight_nodes);
            void SetObjectUniforms(const glm::mat4& model);

            std::shared_ptr<Core::Program> GetProgram() override;

            const int mapWidth, mapHeight, numLayers;
            const std::shared_ptr<Core::Tex3D> depthMap;
            
        private:
            Core::Fbo fbo;

            void setGlobalUniforms(Component::DirectionalLight& dl, GLuint depth_texture, int* shadow_idx);
            
            inline static std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_shadow2d.vs", "shaders/shaderf_shadow2d.fs");
            
    };
        
    class PointShadowModule : public Module {
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

    class LightingPassPBRModule : public Module {
        public:
            LightingPassPBRModule();
            void SetGlobalUniforms();

            std::shared_ptr<Core::Program> GetProgram() override;

            int gPosition, gNormal, gAlbedoSpec, gMetRouOcc;
            int ssao;
            int irradianceMap, prefilterMap, brdfLUT;
            float ibl;
            int shadowmap_2d_array_shadow, shadowmap_cube_array_shadow;

            inline static std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_lightingpasspbr.fs");
    };

    class SkyboxModule : public Module {
        public:
            SkyboxModule();
            void SetGlobalUniforms(Component::Camera& camera, Core::Tex2D& skybox, int tex_unit);

            std::shared_ptr<Core::Program> GetProgram() override;

            inline static const std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_skybox.vs", "shaders/shaderf_skybox.fs");
    };

    class SsaoModule : public Module {
        public:
            SsaoModule();

            void Render(Core::Tex& positions, Core::Tex& normals, const glm::mat4& projection);

            std::shared_ptr<Core::Program> GetProgram() override;

            const std::shared_ptr<Core::Tex> ssao;
            const std::shared_ptr<Core::Tex> ssaoBlurred;

        private:
            Core::Fbo fbo;
            Core::Tex2D noise;
            std::vector<glm::vec3> kernel;

            inline static const std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dssao.fs");
            inline static const std::shared_ptr<Core::Program> programBlur = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dssaoblur.fs");
    };

}