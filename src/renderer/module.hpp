#pragma once

#include "core/program.hpp"
#include "core/globject.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace Component  {
    class ComponentBase;
    class Camera;
    class DirectionalLight;
    class PointLight;
}

namespace Scene {
    class Scene;
    class SceneNode;
}

namespace Renderer {

    class RenderModule {
        public:
            virtual std::shared_ptr<Core::Program> GetProgram() = 0;
            virtual void SetObjectUniforms(const glm::mat4& model, const Component::ComponentBase& component) {}
            virtual bool AllowDraw(const Component::ComponentBase& component) { return true; }
        protected:
            RenderModule() = default;
            virtual ~RenderModule() = default;
    };

    class DirectionalShadowModule : public RenderModule {
        public:
            DirectionalShadowModule(int map_width = 512, int map_height = 512, int num_layers = 8);

            const int mapWidth, mapHeight, numLayers;
            const std::shared_ptr<Core::Tex3D> depthMap;
            
            std::shared_ptr<Core::Program> GetProgram() override { return program; }
            void SetObjectUniforms(const glm::mat4& model, const Component::ComponentBase& component) override;
            bool AllowDraw(const Component::ComponentBase& component) override;
            void Render(Scene::Scene& scene, const std::vector<Scene::SceneNode*>& dirlight_nodes);
            
        private:
            Core::Fbo fbo;
            inline static std::shared_ptr<Core::Program> program;

            void setGlobalUniforms(Component::DirectionalLight& dl, GLuint depth_texture, int* shadow_idx);
    };
        
    class PointShadowModule : public RenderModule {
        public:
            PointShadowModule(int map_width = 512, int map_height = 512, int num_layers = 8);

            const int mapWidth, mapHeight, numLayers;
            const std::shared_ptr<Core::Tex3D> depthMap;
            
            std::shared_ptr<Core::Program> GetProgram() override { return program; }
            void SetObjectUniforms(const glm::mat4& model, const Component::ComponentBase& component) override;
            bool AllowDraw(const Component::ComponentBase& component) override;
            void Render(Scene::Scene& scene, const std::vector<Scene::SceneNode*>& pointlight_nodes);
            
        private:
            Core::Fbo fbo;
            inline static std::shared_ptr<Core::Program> program;
            
            void setGlobalUniforms(Component::PointLight& pl, const glm::vec3& position, int* shadow_idx);
        };

    class SkyboxModule : public RenderModule {
        public:
            SkyboxModule();

            std::shared_ptr<Core::Program> GetProgram() override { return program; }
            void Render(Core::Tex2D& skybox, Component::Camera& camera);
            
        private:
            inline static std::shared_ptr<Core::Program> program;
                
            void setGlobalUniforms(Component::Camera& camera, int tex_unit);

    };

    class SsaoModule : public RenderModule {
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