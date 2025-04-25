#pragma once

#include <plum/core/globject.hpp>
#include <plum/core/tex.hpp>

#include <plum/scene/scene.hpp>
#include <plum/scene/scenenode.hpp>
#include <plum/scene/environment.hpp>
#include <plum/component/camera.hpp>
#include <plum/component/light.hpp>

#include <plum/renderer/module.hpp>

#include <plum/context/inputsevents.hpp>
#include <plum/context/window.hpp>

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