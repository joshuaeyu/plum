#pragma once

#include <plum/material/module.hpp>
#include <plum/material/program.hpp>
#include <plum/component/texture.hpp>

namespace Material {

    class Environment {
        public:

            Environment(std::shared_ptr<Component::Texture> envmap);

            std::shared_ptr<Component::Texture> skybox;
            std::shared_ptr<Component::Texture> irradiance;
            std::shared_ptr<Component::Texture> prefilter;
            std::shared_ptr<Component::Texture> brdfLut;

            SkyboxModule skyboxModule;

        private:

            std::shared_ptr<Component::Texture> equirectToCubemap(std::shared_ptr<Component::Texture> equirect, const unsigned int width, const unsigned int height);
            void cubemapToIrradiance(const unsigned int width, const unsigned int height);
            void cubemapToPrefilter(const unsigned int width, const unsigned int height, const unsigned int envres);
            void generateBrdfLut(const unsigned int width, const unsigned int height);

            inline static const std::shared_ptr<Program> equirectProgram = std::make_shared<Program>("shaders/shaderv_equirect.vs", "shaders/shaderf_equirect.fs");

            inline static const std::shared_ptr<Program> irradianceProgram = std::make_shared<Program>("shaders/shaderv_equirect.vs", "shaders/shaderf_irradiance.fs");

            inline static const std::shared_ptr<Program> prefilterProgram = std::make_shared<Program>("shaders/shaderv_equirect.vs", "shaders/shaderf_prefilter.fs");
            
            inline static const std::shared_ptr<Program> brdfLutProgram = std::make_shared<Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dbrdflut.fs");
    };

}