#pragma once

#include "component/camera.hpp"
#include "core/program.hpp"
#include "core/tex.hpp"
#include "material/texture.hpp"
#include "renderer/module.hpp"

namespace Scene {

    class Environment {
        public:
            Environment();

            float iblIntensity = 1.0f;
            std::shared_ptr<Material::Texture> envmap;
            std::shared_ptr<Core::Tex2D> skybox;
            std::shared_ptr<Core::Tex2D> irradiance;
            std::shared_ptr<Core::Tex2D> prefilter;
            std::shared_ptr<Core::Tex2D> brdfLut;

            void DrawSkybox(Component::Camera& camera);
            void Setup(std::shared_ptr<Material::Texture> env_map);

            void DisplayWidget();

        private:
            Renderer::SkyboxModule skyboxModule;
            inline static std::shared_ptr<Core::Program> equirectProgram;
            inline static std::shared_ptr<Core::Program> irradianceProgram;
            inline static std::shared_ptr<Core::Program> prefilterProgram;
            inline static std::shared_ptr<Core::Program> brdfLutProgram;

            std::shared_ptr<Core::Tex2D> equirectToCubemap(std::shared_ptr<Core::Tex2D> equirect, int width, int height);
            void cubemapToIrradiance(int width, int height);
            void cubemapToPrefilter(int width, int height, int envres);
            void generateBrdfLut(int width, int height);
    };

}