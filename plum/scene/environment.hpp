#pragma once

#include <plum/core/program.hpp>
#include <plum/core/tex.hpp>
#include <plum/material/module.hpp>

namespace Scene {

    class Environment {
        public:

            Environment();
            Environment(std::shared_ptr<Core::Tex2D> envmap);

            std::shared_ptr<Core::Tex2D> skybox;
            std::shared_ptr<Core::Tex2D> irradiance;
            std::shared_ptr<Core::Tex2D> prefilter;
            std::shared_ptr<Core::Tex2D> brdfLut;

            Material::SkyboxModule skyboxModule;

        private:

            std::shared_ptr<Core::Tex2D> equirectToCubemap(std::shared_ptr<Core::Tex2D> equirect, unsigned int width, unsigned int height);
            void cubemapToIrradiance(unsigned int width, unsigned int height);
            void cubemapToPrefilter(unsigned int width, unsigned int height, unsigned int envres);
            void generateBrdfLut(unsigned int width, unsigned int height);

            inline static const std::shared_ptr<Core::Program> equirectProgram = std::make_shared<Core::Program>("shaders/shaderv_equirect.vs", "shaders/shaderf_equirect.fs");

            inline static const std::shared_ptr<Core::Program> irradianceProgram = std::make_shared<Core::Program>("shaders/shaderv_equirect.vs", "shaders/shaderf_irradiance.fs");

            inline static const std::shared_ptr<Core::Program> prefilterProgram = std::make_shared<Core::Program>("shaders/shaderv_equirect.vs", "shaders/shaderf_prefilter.fs");
            
            inline static const std::shared_ptr<Core::Program> brdfLutProgram = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dbrdflut.fs");
    };

}