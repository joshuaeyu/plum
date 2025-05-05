#pragma once

#include "component/camera.hpp"
#include "renderer/postprocessing.hpp"
#include "renderer/renderer.hpp"
#include "scene/environment.hpp"
#include "scene/scene.hpp"

#include <string>

class Demo {
    public:
        Demo(const std::string& title);
        virtual ~Demo() = default;

        const std::string title;
        bool shouldEnd = false;

        void Initialize();
        void DisplayScene();
        void DisplayGui();
        void CleanUp();
    
    protected:
        struct RenderOptions {
            bool hdr = true;
            float hdrExposure = 1.0f;
            bool ssao = true;
            bool fxaa = true;
            bool bloom = false;
        } renderOptions;
        
        Core::Fbo* fbo;

        std::unique_ptr<Scene::Scene> scene;
        std::unique_ptr<Scene::Environment> environment;
        std::unique_ptr<Component::Camera> camera;
        
        std::unique_ptr<Renderer::DeferredRenderer> renderer;
        std::unique_ptr<PostProcessing::Fxaa> fxaa;
        std::unique_ptr<PostProcessing::Hdr> hdr;
        std::unique_ptr<PostProcessing::Bloom> bloom;

        virtual void initialize() {}
        virtual void preDisplayScene() {}
        virtual void postDisplayScene() {}
        virtual void displayGui() {}
        virtual void cleanUp() {}
};