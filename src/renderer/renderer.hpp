#pragma once

#include "core/globject.hpp"
#include "core/tex.hpp"

#include "scene/scene.hpp"
#include "scene/scenenode.hpp"
#include "scene/environment.hpp"
#include "component/camera.hpp"
#include "component/light.hpp"

#include "renderer/module.hpp"

#include "context/inputsevents.hpp"
#include "context/window.hpp"

#include <vector>
#include <memory>

namespace Renderer {

    class RendererBase {
        protected:
            RendererBase();
            virtual ~RendererBase() = default;
            
            std::shared_ptr<Context::Window> window;
            
            virtual Core::Fbo* Render(Scene::Scene& scene, Component::Camera& camera) = 0;
    };

    class DeferredRenderer : public RendererBase {
        
        public:
            DeferredRenderer();
            ~DeferredRenderer();

            bool ssao = true;
            
            Core::Fbo* Render(Scene::Scene& scene, Component::Camera& camera) override;
            Core::Fbo* Render(Scene::Scene& scene, Component::Camera& camera, Scene::Environment& env);

        private:  
            Core::Fbo gBuffer;
            Core::Fbo output;
            
            SsaoModule ssaoModule;
            DirectionalShadowModule dirShadowModule;
            PointShadowModule pointShadowModule;

            inline static std::shared_ptr<Core::Program> lightingPassProgram;

            std::shared_ptr<Core::Ubo> uboVsMatrices;
            std::shared_ptr<Core::Ubo> uboFsMatrices;
            std::shared_ptr<Core::Ubo> uboFsCamera;
            std::shared_ptr<Core::Ubo> uboFsDirlight;
            std::shared_ptr<Core::Ubo> uboFsPointlight;

            std::vector<Scene::SceneNode*> directionalLightNodes;
            std::vector<Scene::SceneNode*> pointLightNodes;

            std::shared_ptr<Context::EventListener> eventListener;
            
            // Setup
            void initUniformBlocks();
            void initGbuffer();
            void initOutput();
            
            // Per frame
            void parseLights(Scene::Scene& scene);
            void updateGlobalUniforms(Scene::Scene& scene, Component::Camera& camera);
            void setDirectionalLightUniforms(Component::Camera& camera);
            void setPointLightUniforms(Component::Camera& camera);
            void geometryPass(Scene::Scene& scene);
            void ssaoPass(Component::Camera& camera);
            void shadowMapPass(Scene::Scene& scene);
            void lightingPass(Scene::Environment& env);
            void forwardPass(Component::Camera& camera, Scene::Environment& env);

            void framebufferSizeCallback(int width, int height);
    };

}