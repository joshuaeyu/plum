#pragma once

#include <plum/component/core.hpp>
#include <plum/component/scene.hpp>
#include <plum/component/scenenode.hpp>
#include <plum/component/light.hpp>
#include <plum/component/texture.hpp>

#include <plum/camera.hpp>

namespace Renderer {

    class BaseRenderer {
        protected:
            BaseRenderer();
            virtual ~BaseRenderer();
            virtual void Render(const Component::Scene& scene) = 0;
    };

    class DeferredRenderer : public BaseRenderer {
        
        public:
            DeferredRenderer();
            ~DeferredRenderer();

            Component::Fbo Render(Component::Scene& scene, Camera& camera);
            // does scene need anything that the nodes can't support
            // should camera be "attached" to scene or passed in each render call

            // probably need a dedicated class/system for uniform block structure

            // how entangled should renderer/material/shaders be
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
            void InitializeUniformBlocks();
            void SetUniforms(Component::Scene& scene, Camera& camera);
            void SetDirectionalLightUniforms(Component::DirectionalLight& dl, Camera& camera);
            void SetPointLightUniforms(Component::PointLight& pl, Camera& camera);

            void InitGbuffer();
            void InitShadowMaps();

            void GeometryPass();
            void ShadowMapPass();
            // void SSAOPass();
            void LightingPass();
            void ForwardPass();

            Component::Fbo gBuffer;
            Component::Fbo dirShadowBuffer;
            Component::Fbo pointShadowBuffer;

            Component::Fbo output;

            std::shared_ptr<Component::Ubo> uboVsMatrices;
            std::shared_ptr<Component::Ubo> uboFsMatrices;
            std::shared_ptr<Component::Ubo> uboFsCamera;
            std::shared_ptr<Component::Ubo> uboFsDirlight;
            std::shared_ptr<Component::Ubo> uboFsPointlight;
    };

}