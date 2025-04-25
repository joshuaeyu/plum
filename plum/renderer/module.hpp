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
    class Scene;
    class SceneNode;
}

namespace Renderer {

    class Module {
        public:
            virtual std::shared_ptr<Core::Program> GetProgram() = 0;
            virtual void SetObjectUniforms(const glm::mat4& model) {}
        protected:
            Module() = default;
            virtual ~Module() = default;
    };

    // shadow module as its own thing, to support 3D texture
    // shadowmodule.setparams()
    // shadowmodule.render(scene, lights = {})

    // shadow module per light like three.js, only supports 2D textures?
    // light.shadow.setparams(...)
    // light.shadow.render(scene)
    // light.shadow.getmap()

    class DirectionalShadowModule : public Module {
        public:
            DirectionalShadowModule(int map_width = 512, int map_height = 512, int num_layers = 8);

            const int mapWidth, mapHeight, numLayers;
            const std::shared_ptr<Core::Tex3D> depthMap;
            
            std::shared_ptr<Core::Program> GetProgram() override { return program; }
            void SetObjectUniforms(const glm::mat4& model) override;
            void Render(Scene::Scene& scene, const std::vector<Scene::SceneNode*>& dirlight_nodes);
            
        private:
            Core::Fbo fbo;
            inline static std::shared_ptr<Core::Program> program;

            void setGlobalUniforms(Component::DirectionalLight& dl, GLuint depth_texture, int* shadow_idx);
    };
        
    class PointShadowModule : public Module {
        public:
            PointShadowModule(int map_width = 512, int map_height = 512, int num_layers = 8);

            const int mapWidth, mapHeight, numLayers;
            const std::shared_ptr<Core::Tex3D> depthMap;
            
            std::shared_ptr<Core::Program> GetProgram() override { return program; }
            void SetObjectUniforms(const glm::mat4& model) override;
            void Render(Scene::Scene& scene, const std::vector<Scene::SceneNode*>& pointlight_nodes);
            
        private:
            Core::Fbo fbo;
            inline static std::shared_ptr<Core::Program> program;
            
            void setGlobalUniforms(Component::PointLight& pl, const glm::vec3& position, int* shadow_idx);
        };

    class SkyboxModule : public Module {
        public:
            SkyboxModule();

            std::shared_ptr<Core::Program> GetProgram() override { return program; }
            void Render(Core::Tex2D& skybox, Component::Camera& camera);
            
        private:
            inline static std::shared_ptr<Core::Program> program;
                
            void setGlobalUniforms(Component::Camera& camera, int tex_unit);

    };

    class SsaoModule : public Module {
        public:
            SsaoModule();

            const std::shared_ptr<Core::Tex2D> ssao;
            const std::shared_ptr<Core::Tex2D> ssaoBlurred;
            
            std::shared_ptr<Core::Program> GetProgram() override { return program; }
            void Render(Core::Tex& positions, Core::Tex& normals, const glm::mat4& projection);

        private:
            Core::Fbo fbo;
            Core::Tex2D noise;
            std::vector<glm::vec3> kernel;
            inline static std::shared_ptr<Core::Program> program;
            inline static std::shared_ptr<Core::Program> programBlur;
    };

}