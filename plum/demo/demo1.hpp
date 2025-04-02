#pragma once

#include <plum/demo/demo.hpp>

#include <plum/component/all.hpp>
#include <plum/context/all.hpp>
#include <plum/core/all.hpp>
#include <plum/renderer/all.hpp>
#include <plum/scene/all.hpp>

class Demo1 : public Demo {
    public:
        Demo1();

        void Initialize() override;
        void Display() override;
        void CleanUp() override;

        struct RenderOptions {
            float ibl = 1.0f;
            bool ssao = true;
            bool fxaa = true;
            bool hdr = true;
            bool bloom = true;
        } renderOptions;

    private:
        void displayGui();

        std::unique_ptr<Scene::Scene> scene;
        std::unique_ptr<Scene::Environment> environment;
        std::unique_ptr<Component::Camera> camera;
        
        std::unique_ptr<Renderer::DeferredRenderer> renderer;
        std::unique_ptr<PostProcessing::Fxaa> fxaa;
        std::unique_ptr<PostProcessing::Hdr> hdr;
        std::unique_ptr<PostProcessing::Bloom> bloom;

    public:
        ~Demo1() = default;
};