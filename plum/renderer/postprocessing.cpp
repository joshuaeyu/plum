#include <plum/renderer/postprocessing.hpp>
#include <plum/component/primitive.hpp>
#include <iostream>
namespace PostProcessing {

    PostProcessor::PostProcessor()
        : fbo(2,2)
    {}

    Core::Fbo* PostProcessor::Process(Core::Fbo& input, const int att_idx) {
        return Process(*input.colorAtts[att_idx], input.width, input.height);
    }

    Fxaa::Fxaa()
    {
        auto colorAtt = std::make_shared<Core::Tex2D>(GL_TEXTURE_2D, GL_RGBA32F, fbo.width, fbo.height, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST, false, true);
        
        fbo.Bind();
        fbo.AttachColorTex(colorAtt);
        fbo.AttachDepthRbo16();
        fbo.CheckStatus();
    }

    Core::Fbo* Fxaa::Process(Core::Tex& input, const int width, const int height) {
        if (fbo.width != width || fbo.height != height) {
            fbo.Resize(width, height);
            fbo.CheckStatus();
        }
        fbo.Bind();
        fbo.SetViewportDims();
        fbo.ClearColor();
        fbo.ClearDepth();
        
        program->Use();
        program->SetInt("screenTexture", 0);
        input.Bind(0);
        Component::Primitive::DrawQuad();

        return &fbo;
    }

    Core::Fbo* Bloom::Process(Core::Tex& input, const int width, const int height) {   
        if (fbo.width != width || fbo.height != height) {
            fbo.Resize(width, height);
        }
        fbo.SetViewportDims();
        fbo.ClearColor();
        fbo.ClearDepth();

        // // Render to Bloom2 using Bloom1 first, then to Bloom1 using Bloom2, swapping repeatedly
        // const int amount = 3;
        // bool horizontal = true;
        // for (int i = 0; i < amount*2; i++, horizontal = !horizontal) {
        //     Engine::Framebuffer drawFb, readFb;
        //     if (!horizontal) {
        //         drawFb = engine->Bloom1;
        //         readFb = engine->Bloom2;
        //     } else {
        //         drawFb = engine->Bloom2;
        //         readFb = engine->Bloom1;
        //     }
        //     glBindFramebuffer(GL_FRAMEBUFFER, drawFb.fbo);
        //     shader_2dbloomblur.setInt("horizontal", horizontal);
        //     glBindTexture(GL_TEXTURE_2D, readFb.colors[1]);   // Use raw HDR scene highlights first, then ping pong
        //     engine->RenderQuad();
        //     horizontal = !horizontal;
        // }
        // glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.front());
        // framebuffers.pop();
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // program->Use();
        // program->SetInt("sceneRaw", 0);
        // program->SetInt("sceneBloom", 1);
        //     glBindTexture(GL_TEXTURE_2D, engine->Bloom1.colors[0]);
        //     glActiveTexture(GL_TEXTURE1);
        //     glBindTexture(GL_TEXTURE_2D, engine->Bloom1.colors[1]);
        //     Component::Primitive::DrawQuad();
        // program->SetInt("screenTexture", 0);
        // input.Bind(0);

        return &fbo;
    }

    Core::Fbo* Ssao::Process(Core::Tex& input, const int width, const int height) {
        return nullptr;
    }

    Hdr::Hdr()
    {
        auto colorAtt = std::make_shared<Core::Tex2D>(GL_TEXTURE_2D, GL_RGBA32F, fbo.width, fbo.height, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST, false, true);
        
        fbo.Bind();
        fbo.AttachColorTex(colorAtt);
        fbo.AttachDepthRbo16();
        fbo.CheckStatus();
    }

    Core::Fbo* Hdr::Process(Core::Tex& input, const int width, const int height) {
        if (fbo.width != width || fbo.height != height) {
            fbo.Resize(width, height);
        }
        fbo.Bind();
        fbo.SetViewportDims();
        fbo.ClearColor();
        fbo.ClearDepth();
        
        program->Use();
        program->SetFloat("exposure", 1.0f);
        program->SetInt("hdrBuffer", 0);
        input.Bind(0);
        Component::Primitive::DrawQuad();

        return &fbo;
    }

}