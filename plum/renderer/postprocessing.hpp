#pragma once

#include <plum/core/globject.hpp>
#include <plum/core/program.hpp>

namespace PostProcessing {

    class PostProcessor {
        public:
            virtual ~PostProcessor() = default;
            virtual Core::Fbo* Process(Core::Tex& input, int width, int height) = 0;
            Core::Fbo* Process(Core::Fbo& input, int att_idx = 0);
        
        protected:
            Core::Fbo output;
            PostProcessor();
    };

    class Fxaa : public PostProcessor {
        public:
            Fxaa();

            using PostProcessor::Process;
            Core::Fbo* Process(Core::Tex& input, int width, int height) override;
        
        private:
            inline static std::shared_ptr<Core::Program> program;
    };

    class Bloom : public PostProcessor {
        public:
            Bloom();

            using PostProcessor::Process;
            Core::Fbo* Process(Core::Tex& input, int width, int height) override;
            
            Core::Fbo highlights;
            Core::Fbo bloom;
        
        private:
            inline static std::shared_ptr<Core::Program> programHighlights;
            inline static std::shared_ptr<Core::Program> programBlur;
            inline static std::shared_ptr<Core::Program> programDisplay;
    };

    class Hdr : public PostProcessor {
        public:
            Hdr();    

            using PostProcessor::Process;
            Core::Fbo* Process(Core::Tex& input, int width, int height) override;

            float exposure = 1.0;
        
        private:
            inline static std::shared_ptr<Core::Program> program;
    };

}