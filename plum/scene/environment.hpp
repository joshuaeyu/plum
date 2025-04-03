#pragma once

#include <plum/component/camera.hpp>
#include <plum/core/program.hpp>
#include <plum/core/tex.hpp>
#include <plum/renderer/module.hpp>

namespace Scene {

    class Environment {
        public:
            Environment();
            Environment(std::shared_ptr<Core::Tex2D> envmap);

            void DrawSkybox(Component::Camera& camera);

            float iblIntensity = 1.0f;

            std::shared_ptr<Core::Tex2D> skybox;
            std::shared_ptr<Core::Tex2D> irradiance;
            std::shared_ptr<Core::Tex2D> prefilter;
            std::shared_ptr<Core::Tex2D> brdfLut;

        private:
            Renderer::SkyboxModule skyboxModule;

            std::shared_ptr<Core::Tex2D> equirectToCubemap(std::shared_ptr<Core::Tex2D> equirect, int width, int height);
            void cubemapToIrradiance(int width, int height);
            void cubemapToPrefilter(int width, int height, int envres);
            void generateBrdfLut(int width, int height);

            inline static std::shared_ptr<Core::Program> equirectProgram;
            inline static std::shared_ptr<Core::Program> irradianceProgram;
            inline static std::shared_ptr<Core::Program> prefilterProgram;
            inline static std::shared_ptr<Core::Program> brdfLutProgram;
    };

}