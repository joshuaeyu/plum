#pragma once

#include "component/camera.hpp"
#include "renderer/postprocessing.hpp"
#include "renderer/renderer.hpp"
#include "scene/environment.hpp"
#include "scene/scene.hpp"

#include <string>

class Demo {
    public:
        Demo(const std::string& title) : title(title) { baseSetup(); }
        virtual ~Demo() = default;

        const std::string title;

        struct RenderOptions {
            float iblIntensity = 1.0f;
            bool ssao = true;
            bool fxaa = true;
            bool hdr = true;
            float hdrExposure = 1.0f;
            bool bloom = false;
        } renderOptions;

        bool ShouldEnd() const { return shouldEnd; }
        
        virtual void Initialize() = 0;
        virtual void Display() = 0;
        virtual void CleanUp() = 0;
    
    protected:
        bool shouldEnd = false;

        std::unique_ptr<Scene::Scene> scene;
        std::unique_ptr<Scene::Environment> environment;
        std::unique_ptr<Component::Camera> camera;
        
        std::unique_ptr<Renderer::DeferredRenderer> renderer;
        std::unique_ptr<PostProcessing::Fxaa> fxaa;
        std::unique_ptr<PostProcessing::Hdr> hdr;
        std::unique_ptr<PostProcessing::Bloom> bloom;

        void baseSetup();
        virtual void displayMainGui();
};