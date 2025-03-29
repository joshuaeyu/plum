#include <plum/scene/environment.hpp>

#include <plum/core/core.hpp>
#include <plum/component/primitive.hpp>

#include <glm/glm.hpp>

#include <iostream>

namespace Scene {

    Environment::Environment() {}

    Environment::Environment(std::shared_ptr<Core::Tex2D> envmap) 
    {
        if (envmap->target == GL_TEXTURE_CUBE_MAP) {
            skybox = envmap;
            skybox->GenerateMipMap();
        } else if (envmap->target == GL_TEXTURE_2D) {
            skybox = equirectToCubemap(envmap, 2048, 2048);
        } else
            exit(-1);
        cubemapToIrradiance(64, 64);
        cubemapToPrefilter(256, 256, skybox->width);
        generateBrdfLut(512, 512);
    }

    std::shared_ptr<Core::Tex2D> Environment::equirectToCubemap(std::shared_ptr<Core::Tex2D> equirect, const unsigned int width, const unsigned int height) {
        
        Core::Fbo fbo(width, height);
        auto cubemap = std::make_shared<Core::Tex2D>(GL_TEXTURE_CUBE_MAP, GL_RGB32F, fbo.width, fbo.height, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, false, equirect->isHdr);

        fbo.Bind();
        fbo.AttachColorTex(cubemap);
        fbo.AttachDepthRbo24();
        fbo.CheckStatus();

        const glm::mat4 projection = glm::perspective(glm::half_pi<float>(), 1.0f, 0.1f, 10.0f);
        const glm::mat4 views[] = {
            glm::lookAt(glm::vec3(0), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };
        equirectProgram->Use();
        equirectProgram->SetInt("equirectMap", 0);
        equirectProgram->SetMat4("projection", projection);
        equirect->Bind(0);

        static Component::Cube cube(1,1);

        fbo.Bind();
        fbo.SetViewportDims();
        for (int i = 0; i < 6; i++) {
            fbo.AttachColorTexCubeFace(0, i);
            fbo.ClearColor();
            fbo.ClearDepth();
            equirectProgram->SetMat4("view", views[i]);
            glCullFace(GL_FRONT);
            cube.vao->Draw();
            glCullFace(GL_BACK);
        }

        cubemap->GenerateMipMap();

        return cubemap;
    }

    void Environment::cubemapToIrradiance(const unsigned int width, const unsigned int height) {
       
        Core::Fbo fbo(width, height);
        irradiance = std::make_shared<Core::Tex2D>(GL_TEXTURE_CUBE_MAP, GL_RGB32F, fbo.width, fbo.height, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, false, true);

        fbo.Bind();
        fbo.AttachColorTex(irradiance);
        fbo.AttachDepthRbo24();
        fbo.CheckStatus();
    
        const glm::mat4 projection = glm::perspective(glm::half_pi<float>(), 1.0f, 0.1f, 10.0f);
        const glm::mat4 views[] = {
            glm::lookAt(glm::vec3(0), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };
    
        irradianceProgram->Use();
        irradianceProgram->SetInt("envCubemap", 0);
        irradianceProgram->SetMat4("projection", projection);
        skybox->Bind(0);
        
        static Component::Cube cube(1,1);
    
        fbo.Bind();
        fbo.SetViewportDims();
        for (int i = 0; i < 6; i++) {
            fbo.AttachColorTexCubeFace(0, i);
            fbo.ClearColor();
            fbo.ClearDepth();
            irradianceProgram->SetMat4("view", views[i]);
            glCullFace(GL_FRONT);
            cube.vao->Draw();
            glCullFace(GL_BACK);
        }
    }

    void Environment::cubemapToPrefilter(const unsigned int width, const unsigned int height, const unsigned int envres) {

        Core::Fbo fbo(width, height);
        prefilter = std::make_shared<Core::Tex2D>(GL_TEXTURE_CUBE_MAP, GL_RGB32F, fbo.width, fbo.height, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, false, true);
        prefilter->Bind();
        prefilter->GenerateMipMap();
        fbo.Bind();
        fbo.AttachColorTex(prefilter);
        fbo.AttachDepthRbo24();
        fbo.CheckStatus();
        
        const glm::mat4 projection = glm::perspective(glm::half_pi<float>(), 1.0f, 0.1f, 10.0f);
        const glm::mat4 views[] = {
            glm::lookAt(glm::vec3(0), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };
        
        prefilterProgram->Use();
        prefilterProgram->SetInt("envCubemap", 0);
        prefilterProgram->SetMat4("projection", projection);
        prefilterProgram->SetInt("envResolution", envres);
        skybox->Bind(0);
        
        static Component::Cube cube(1,1);
        
        fbo.Bind();
        int maxMipLevels = 5;
        for (int mip = 0; mip < maxMipLevels; mip++) {
            
            int mipWidth = fbo.width * std::pow(0.5, mip);
            int mipHeight = fbo.height * std::pow(0.5, mip);
            
            fbo.depthRboAtt->Bind();
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);
            float roughness = (float)mip / (float)(maxMipLevels - 1);
            prefilterProgram->SetFloat("roughness", roughness);
            
            fbo.CheckStatus();
            for (int i = 0; i < 6; i++) {
                fbo.AttachColorTexCubeFace(0, i, mip);
                fbo.ClearColor();
                fbo.ClearDepth();
                prefilterProgram->SetMat4("view", views[i]);
                glCullFace(GL_FRONT);
                cube.vao->Draw();
                glCullFace(GL_BACK);
            }
        }
    }

    void Environment::generateBrdfLut(const unsigned int width, const unsigned int height) {

        Core::Fbo fbo(width, height);
        brdfLut = std::make_shared<Core::Tex2D>(GL_TEXTURE_2D, GL_RG16F, fbo.width, fbo.height, GL_RG, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, false, true);
        
        fbo.Bind();
        fbo.AttachColorTex(brdfLut);
        fbo.AttachDepthRbo24();
        
        fbo.CheckStatus();

        fbo.SetViewportDims();
        fbo.ClearColor();
        fbo.ClearDepth();
        
        brdfLutProgram->Use();
        Component::Primitive::DrawQuad();
    }

};