#pragma once

#include <plum/demo/demo.hpp>

#include <plum/component/all.hpp>
#include <plum/context/all.hpp>
#include <plum/core/all.hpp>
#include <plum/renderer/all.hpp>
#include <plum/scene/all.hpp>

#include <set>

class Demo1 : public Demo {
    public:
        Demo1();
        ~Demo1() = default;

        struct RenderOptions {
            float iblIntensity = 1.0f;
            bool ssao = true;
            bool fxaa = true;
            bool hdr = true;
            float hdrExposure = 1.0f;
            bool bloom = false;
        } renderOptions;

        void Initialize() override;
        void Display() override;
        void CleanUp() override;

    private:
        void displayGui();

        std::unique_ptr<Scene::Scene> scene;
        std::unique_ptr<Scene::Environment> environment;
        std::unique_ptr<Component::Camera> camera;
        
        std::unique_ptr<Renderer::DeferredRenderer> renderer;
        std::unique_ptr<PostProcessing::Fxaa> fxaa;
        std::unique_ptr<PostProcessing::Hdr> hdr;
        std::unique_ptr<PostProcessing::Bloom> bloom;

        std::set<std::shared_ptr<Material::MaterialBase>> materials;
};