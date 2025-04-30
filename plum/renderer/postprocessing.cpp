#include <plum/renderer/postprocessing.hpp>
#include <plum/component/primitive.hpp>
#include <iostream>

namespace PostProcessing {

    PostProcessor::PostProcessor()
        : output(2,2)
    {}

    Core::Fbo* PostProcessor::Process(Core::Fbo& input, int att_idx) {
        return Process(*input.colorAtts[att_idx], input.width, input.height);
    }

    Fxaa::Fxaa()
    {
        if (!program) {
            AssetManager& manager = AssetManager::Instance();
            auto vs = manager.LoadHot<ShaderAsset>("shaders/shaderv_2d.vs", GL_VERTEX_SHADER);
            auto fs = manager.LoadHot<ShaderAsset>("shaders/shaderf_2dfxaa.fs", GL_FRAGMENT_SHADER);
            
            program = std::make_shared<Core::Program>(vs, fs);
            program->SetUniformBlockBindingScheme(Core::Program::UboScheme::Scheme1);
        }

        auto color = std::make_shared<Core::Tex2D>(GL_TEXTURE_2D, GL_RGBA32F, output.width, output.height, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, false, true);
        
        output.Bind();
        output.AttachColorTex(color);
        output.AttachDepthRbo16();
        output.CheckStatus();
    }

    Core::Fbo* Fxaa::Process(Core::Tex& input, int width, int height) {
        if (output.width != width || output.height != height) {
            output.Resize(width, height);
        }
        output.Bind();
        output.SetViewportDims();
        output.ClearColor();
        output.ClearDepth();
        
        program->Use();
        program->SetInt("screenTexture", 0);
        input.Bind(0);
        Component::Primitive::DrawQuad();

        return &output;
    }

    Bloom::Bloom()
        : highlights(2,2),
        bloom(2,2)
    {
        if (!programHighlights) {
            AssetManager& manager = AssetManager::Instance();

            auto vs2d = manager.LoadHot<ShaderAsset>("shaders/shaderv_2d.vs", GL_VERTEX_SHADER);
            auto fsHighlights = manager.LoadHot<ShaderAsset>("shaders/shaderf_2dhighlights.fs", GL_FRAGMENT_SHADER);
            auto fsGaussian = manager.LoadHot<ShaderAsset>("shaders/shaderf_2dgaussian.fs", GL_FRAGMENT_SHADER);
            auto fsBloom = manager.LoadHot<ShaderAsset>("shaders/shaderf_2dbloom.fs", GL_FRAGMENT_SHADER);
            
            programHighlights = std::make_shared<Core::Program>(vs2d, fsHighlights);
            programBlur = std::make_shared<Core::Program>(vs2d, fsGaussian);
            programDisplay = std::make_shared<Core::Program>(vs2d, fsBloom);
        }

        auto color = std::make_shared<Core::Tex2D>(GL_TEXTURE_2D, GL_RGBA32F, output.width, output.height, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, false, true);
        
        output.Bind();
        output.AttachColorTex(color);
        output.AttachDepthRbo16();
        output.CheckStatus();
        
        auto color1 = std::make_shared<Core::Tex2D>(GL_TEXTURE_2D, GL_RGBA32F, output.width, output.height, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, false, true);
        auto color2 = std::make_shared<Core::Tex2D>(GL_TEXTURE_2D, GL_RGBA32F, output.width, output.height, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, false, true);
        
        bloom.Bind();
        bloom.AttachColorTex(color1);
        bloom.AttachColorTex(color2);
        bloom.AttachDepthRbo16();
        bloom.CheckStatus();
    }

    Core::Fbo* Bloom::Process(Core::Tex& input, int width, int height) {   
        if (output.width != width || output.height != height) {
            output.Resize(width, height);
        }
        if (bloom.width != width || bloom.height != height) {
            bloom.Resize(width, height);
        }

        // Extract highlights from scene
        bloom.Bind();
        bloom.SetViewportDims();
        bloom.ClearColor();
        bloom.ClearDepth();
        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        programHighlights->Use();
        programHighlights->SetInt("rawScene", 0);
        input.Bind(0);
        Component::Primitive::DrawQuad();

        // Ping pong between color attachments 0 and 1 to achieve Gaussian blur
        const int amount = 6;
        bool horizontal = true;

        programBlur->Use();
        programBlur->SetInt("image", 0);
        for (int i = 0; i < amount; i++) {
            const int readIdx = !horizontal;
            const int drawIdx = horizontal;
            programBlur->SetInt("horizontal", horizontal);
            bloom.colorAtts[readIdx]->Bind(0); // Sample from color attachment at readIdx
            glDrawBuffer(GL_COLOR_ATTACHMENT0 + drawIdx); // Draw to color attachment at drawIdx
            Component::Primitive::DrawQuad();
            horizontal = !horizontal;
        }
        
        output.Bind();
        output.SetViewportDims();
        output.ClearColor();
        output.ClearDepth();

        programDisplay->Use();
        programDisplay->SetFloat("intensity", 0.75f);
        programDisplay->SetInt("sceneRaw", 0);
        programDisplay->SetInt("sceneBloom", 1);
        input.Bind(0);
        bloom.colorAtts[amount % 2]->Bind(1);
        Component::Primitive::DrawQuad();

        return &output;
    }

    Hdr::Hdr() 
    {
        if (!program) {
            AssetManager& manager = AssetManager::Instance();
            auto vs = manager.LoadHot<ShaderAsset>("shaders/shaderv_2d.vs", GL_VERTEX_SHADER);
            auto fs = manager.LoadHot<ShaderAsset>("shaders/shaderf_2dhdr.fs", GL_FRAGMENT_SHADER);
            
            program = std::make_shared<Core::Program>(vs, fs);
            program->SetUniformBlockBindingScheme(Core::Program::UboScheme::Scheme1);
        }
        
        auto color = std::make_shared<Core::Tex2D>(GL_TEXTURE_2D, GL_RGBA32F, output.width, output.height, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, false, true);
        
        output.Bind();
        output.AttachColorTex(color);
        output.AttachDepthRbo16();
        output.CheckStatus();
    }

    Core::Fbo* Hdr::Process(Core::Tex& input, int width, int height) {
        if (output.width != width || output.height != height) {
            output.Resize(width, height);
        }
        output.Bind();
        output.SetViewportDims();
        output.ClearColor();
        output.ClearDepth();
        
        program->Use();
        program->SetFloat("exposure", exposure);
        program->SetInt("hdrBuffer", 0);
        input.Bind(0);
        Component::Primitive::DrawQuad();

        return &output;
    }

}