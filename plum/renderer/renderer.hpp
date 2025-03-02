#pragma once

#include <plum/component/core.hpp>
#include <plum/component/scene.hpp>
#include <plum/component/scenenode.hpp>
#include <plum/component/camera.hpp>
#include <plum/component/light.hpp>
#include <plum/component/tex.hpp>
#include <plum/material/module.hpp>
#include <plum/material/environment.hpp>

#include <vector>

#include <plum/context/inputmanager.hpp>

namespace Renderer {

    class BaseRenderer {
        protected:
            BaseRenderer();
            virtual ~BaseRenderer();
            virtual Component::Fbo& Render(Component::Scene& scene, Component::Camera& camera) = 0;
    };

    class DeferredRenderer : public BaseRenderer {
        
        public:
            DeferredRenderer();
            ~DeferredRenderer();

            Component::Fbo& Render(Component::Scene& scene, Component::Camera& camera) override;
            Component::Fbo& Render(Component::Scene& scene, Component::Camera& camera, Material::Environment& env);

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
            void InitShadowMaps();
            
            // Per frame
            void ParseLights(Component::Scene& scene);
            void UpdateGlobalUniforms(Component::Scene& scene, Component::Camera& camera);
            void SetDirectionalLightUniforms(Component::Camera& camera);
            void SetPointLightUniforms(Component::Camera& camera);
            void GeometryPass(Component::Scene& scene);
            void ShadowMapPass(Component::Scene& scene);
            // void SSAOPass();
            void LightingPass(Material::Environment& env);
            void ForwardPass(Component::Camera& camera, Material::Environment& env);

            Component::Fbo gBuffer;
            
            Component::Fbo dirShadowBuffer;
            Material::DirectionalShadowModule dirShadowModule;
            
            Component::Fbo pointShadowBuffer;
            Material::PointShadowModule pointShadowModule;
            
            Component::Fbo output;
            Material::LightingPassPBRModule lightingPassPbrModule;

            std::shared_ptr<Component::Ubo> uboVsMatrices;
            std::shared_ptr<Component::Ubo> uboFsMatrices;
            std::shared_ptr<Component::Ubo> uboFsCamera;
            std::shared_ptr<Component::Ubo> uboFsDirlight;
            std::shared_ptr<Component::Ubo> uboFsPointlight;

            std::vector<Component::SceneNode*> directionalLights;
            std::vector<Component::SceneNode*> pointLights;

            Context::InputManager inputManager;
    };

}