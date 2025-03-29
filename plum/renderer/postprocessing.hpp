#pragma once

#include <plum/core/core.hpp>
#include <plum/core/program.hpp>

namespace PostProcessing {

    class PostProcessor {
        public:
            virtual Core::Fbo* Process(Core::Tex& input, const int width, const int height) = 0;
        protected:
            Core::Fbo fbo;
            Core::Fbo* Process(Core::Fbo& input, const int att_idx = 0);
            PostProcessor();
    };

    class Fxaa : public PostProcessor {
        public:
            Fxaa();

            using PostProcessor::Process;
            Core::Fbo* Process(Core::Tex& input, const int width, const int height) override;
            
            // static Core::Fbo fbo;
            
            inline static std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dfxaa.fs");
    };

    class Bloom : public PostProcessor {
        public:
            Bloom();

            using PostProcessor::Process;
            Core::Fbo* Process(Core::Tex& input, const int width, const int height) override;
            
            // static Core::Fbo fbo;

            inline static std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dbloom.fs");
            inline static std::shared_ptr<Core::Program> programBlur = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dgaussian.fs");
    };

    class Ssao : public PostProcessor {
        public:
            Ssao();

            using PostProcessor::Process;
            Core::Fbo* Process(Core::Tex& input, const int width, const int height) override;
            
            // static Core::Fbo fbo;

            inline static std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dssao.fs");
            inline static std::shared_ptr<Core::Program> programBlur = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dssaoblur.fs");
    };

    class Hdr : public PostProcessor {
        public:
            Hdr();    

            using PostProcessor::Process;
            Core::Fbo* Process(Core::Tex& input, const int width, const int height) override;
            
            // static Core::Fbo fbo;

            inline static std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dhdr.fs");
    };

}