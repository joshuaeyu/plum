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
            RendererBase(std::shared_ptr<Context::Window> window);
            virtual ~RendererBase() = default;
            
            virtual Core::Fbo* Render(Scene::Scene& scene, Component::Camera& camera) = 0;
            
            std::shared_ptr<Context::Window> window;
    };

    class DeferredRenderer : public RendererBase {
        
        public:
            DeferredRenderer(std::shared_ptr<Context::Window> window);
            ~DeferredRenderer();

            Core::Fbo* Render(Scene::Scene& scene, Component::Camera& camera) override;
            Core::Fbo* Render(Scene::Scene& scene, Component::Camera& camera, Scene::Environment& env);

            // does scene need anything that the nodes can't support
            // should camera be "attached" to scene or passed in each render call

            // probably need a dedicated class/system for uniform block structure

            // how tightly coupled should renderer/material/shaders be
            // renderer: forward, deferred, forward+...
            //  - figures out what gets rendered, in what order
            //  - affects order of operations in shaders
            // material: Phong, PBR, wireframe, ...
            //  - what things actually look like
            //  - affects calculations and textures in shaders
            // shader: some kind of preprocessed composite of renderer and material requirements

        private:
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
            
            Core::Fbo gBuffer;
            Core::Fbo output;
            
            SsaoModule ssaoModule;
            DirectionalShadowModule dirShadowModule;
            PointShadowModule pointShadowModule;

            inline static std::shared_ptr<Core::Program> lightingPassProgram = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_lightingpasspbr.fs");

            std::shared_ptr<Core::Ubo> uboVsMatrices;
            std::shared_ptr<Core::Ubo> uboFsMatrices;
            std::shared_ptr<Core::Ubo> uboFsCamera;
            std::shared_ptr<Core::Ubo> uboFsDirlight;
            std::shared_ptr<Core::Ubo> uboFsPointlight;

            std::vector<Scene::SceneNode*> directionalLightNodes;
            std::vector<Scene::SceneNode*> pointLightNodes;

            void framebufferSizeCallback(int width, int height);

            std::shared_ptr<Context::WindowEventListener> eventListener;
    };

}