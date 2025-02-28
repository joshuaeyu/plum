#pragma once

#include <plum/material/module.hpp>
#include <plum/material/program.hpp>
#include <plum/component/tex.hpp>

namespace Material {

    class Environment {
        public:

            Environment(std::shared_ptr<Component::Tex2D> envmap);

            std::shared_ptr<Component::Tex2D> skybox;
            std::shared_ptr<Component::Tex2D> irradiance;
            std::shared_ptr<Component::Tex2D> prefilter;
            std::shared_ptr<Component::Tex2D> brdfLut;

            SkyboxModule skyboxModule;

        private:

            std::shared_ptr<Component::Tex2D> equirectToCubemap(std::shared_ptr<Component::Tex2D> equirect, const unsigned int width, const unsigned int height);
            void cubemapToIrradiance(const unsigned int width, const unsigned int height);
            void cubemapToPrefilter(const unsigned int width, const unsigned int height, const unsigned int envres);
            void generateBrdfLut(const unsigned int width, const unsigned int height);

            inline static const std::shared_ptr<Program> equirectProgram = std::make_shared<Program>("shaders/shaderv_equirect.vs", "shaders/shaderf_equirect.fs");

            inline static const std::shared_ptr<Program> irradianceProgram = std::make_shared<Program>("shaders/shaderv_equirect.vs", "shaders/shaderf_irradiance.fs");

            inline static const std::shared_ptr<Program> prefilterProgram = std::make_shared<Program>("shaders/shaderv_equirect.vs", "shaders/shaderf_prefilter.fs");
            
            inline static const std::shared_ptr<Program> brdfLutProgram = std::make_shared<Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dbrdflut.fs");
    };

}