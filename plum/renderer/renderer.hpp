#pragma once

#include <plum/core/core.hpp>
#include <plum/core/tex.hpp>

#include <plum/scene/scene.hpp>
#include <plum/scene/scenenode.hpp>
#include <plum/scene/environment.hpp>
#include <plum/component/camera.hpp>
#include <plum/component/light.hpp>

#include <plum/material/module.hpp>

#include <plum/context/inputsevents.hpp>
#include <plum/context/window.hpp>

#include <vector>
#include <memory>

namespace Renderer {

    class RendererBase {
        protected:
            RendererBase(std::shared_ptr<Context::Window> window);
            virtual ~RendererBase();
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

            // separate renderer and scene graph, i.e., draw calls should come from renderer
            // renderer.render(scene, camera); // like three.js
            // renderer is what sets uniforms.

        private:
            bool noEnvironment;

            // Setup
            void InitializeUniformBlocks();
            void InitGbuffer();
            void InitOutput();
            
            // Per frame
            void ParseLights(Scene::Scene& scene);
            void UpdateGlobalUniforms(Scene::Scene& scene, Component::Camera& camera);
            void SetDirectionalLightUniforms(Component::Camera& camera);
            void SetPointLightUniforms(Component::Camera& camera);
            void GeometryPass(Scene::Scene& scene);
            void ShadowMapPass(Scene::Scene& scene);
            
            // shadow module as its own thing, to support 3D texture
            // shadowmodule.setparams()
            // shadowmodule.render(scene, lights = {})

            // shadow module per light like three.js, only supports 2D textures?
            // light.shadow.setparams(...)
            // light.shadow.render(scene)
            // light.shadow.getmap()

            // void SSAOPass();
            void LightingPass(Scene::Environment& env);
            void ForwardPass(Component::Camera& camera, Scene::Environment& env);

            Core::Fbo gBuffer;
            
            Material::DirectionalShadowModule dirShadowModule;
            Material::PointShadowModule pointShadowModule;
            
            Core::Fbo output;
            Material::LightingPassPBRModule lightingPassPbrModule;

            std::shared_ptr<Core::Ubo> uboVsMatrices;
            std::shared_ptr<Core::Ubo> uboFsMatrices;
            std::shared_ptr<Core::Ubo> uboFsCamera;
            std::shared_ptr<Core::Ubo> uboFsDirlight;
            std::shared_ptr<Core::Ubo> uboFsPointlight;

            std::vector<Scene::SceneNode*> directionalLightNodes;
            std::vector<Scene::SceneNode*> pointLightNodes;

            void framebufferSizeCallback(int width, int height);

            Context::WindowEventListener eventListener;
    };

}