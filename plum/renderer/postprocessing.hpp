#pragma once

#include <plum/core/globject.hpp>
#include <plum/core/program.hpp>

namespace PostProcessing {

    class PostProcessor {
        public:
            virtual ~PostProcessor() = default;
            virtual Core::Fbo* Process(Core::Tex& input, int width, int height) = 0;
        
        protected:
            Core::Fbo output;
            Core::Fbo* Process(Core::Fbo& input, int att_idx = 0);
            PostProcessor();
    };

    class Fxaa : public PostProcessor {
        public:
            Fxaa();

            using PostProcessor::Process;
            Core::Fbo* Process(Core::Tex& input, int width, int height) override;
            
            inline static std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dfxaa.fs");
    };

    class Bloom : public PostProcessor {
        public:
            Bloom();

            using PostProcessor::Process;
            Core::Fbo* Process(Core::Tex& input, int width, int height) override;
            
            Core::Fbo highlights;
            Core::Fbo bloom;

            inline static std::shared_ptr<Core::Program> programHighlights = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dhighlights.fs");
            inline static std::shared_ptr<Core::Program> programBlur = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dgaussian.fs");
            inline static std::shared_ptr<Core::Program> programDisplay = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dbloom.fs");
    };

    class Ssao : public PostProcessor {
        public:
            Ssao();

            using PostProcessor::Process;
            Core::Fbo* Process(Core::Tex& input, int width, int height) override;
            
            // static Core::Fbo fbo;

            inline static std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dssao.fs");
            inline static std::shared_ptr<Core::Program> programBlur = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dssaoblur.fs");
    };

    class Hdr : public PostProcessor {
        public:
            Hdr();    

            using PostProcessor::Process;
            Core::Fbo* Process(Core::Tex& input, int width, int height) override;

            inline static std::shared_ptr<Core::Program> program = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dhdr.fs");
    };

}