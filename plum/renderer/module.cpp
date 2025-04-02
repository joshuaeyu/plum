#include <plum/renderer/module.hpp>

#include <plum/component/camera.hpp>
#include <plum/component/light.hpp>
#include <plum/component/primitive.hpp>
#include <plum/scene/scene.hpp>
#include <plum/scene/scenenode.hpp>

#include <random>

#include <iostream>

namespace Renderer {

    DirectionalShadowModule::DirectionalShadowModule(int map_width, int map_height, int num_layers)
        : mapWidth(map_width),
        mapHeight(map_height),
        numLayers(num_layers),
        depthMap(std::make_shared<Core::Tex3D>(GL_TEXTURE_2D_ARRAY, GL_DEPTH_COMPONENT24, map_width, map_height, num_layers, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, true)),
        fbo(map_width, map_height)
    {
        if (!program) {
            program = std::make_shared<Core::Program>("shaders/shaderv_shadow2d.vs", "shaders/shaderf_shadow2d.fs");
        }
        fbo.Bind();
        fbo.AttachDepthTex(depthMap);
        glDrawBuffer(GL_NONE);  // No colorbuffer
        glReadBuffer(GL_NONE);  // No colorbuffer
        fbo.CheckStatus();
    }

    void DirectionalShadowModule::Render(Scene::Scene& scene, const std::vector<Scene::SceneNode*>& dirlight_nodes) {
        fbo.Bind();
        fbo.SetViewportDims();
        fbo.ClearDepth();
        program->Use();
        
        for (int i = 0, shadow_count = 0; i < dirlight_nodes.size(); i++) {
            Component::DirectionalLight& dirlight = dynamic_cast<Component::DirectionalLight&>(*dirlight_nodes[i]->component);
            
            setGlobalUniforms(dirlight, fbo.depthAtt->Handle(), &shadow_count);
            
            glCullFace(GL_FRONT);
            scene.Draw(*this);
            glCullFace(GL_BACK);
        }
    }

    void DirectionalShadowModule::setGlobalUniforms(Component::DirectionalLight& dl, GLuint depth_texture, int* shadow_idx) {
        if (!dl.HasShadows())
            return;
        program->SetMat4("lightSpaceMatrix", dl.LightspaceMatrix());
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0, (*shadow_idx)++);
    }

    void DirectionalShadowModule::SetObjectUniforms(const glm::mat4& model) {
        program->SetMat4("model", model);
    }

    std::shared_ptr<Core::Program> DirectionalShadowModule::GetProgram() {
        return program;
    }

    PointShadowModule::PointShadowModule(int map_width, int map_height, int num_layers)
        : mapWidth(map_width),
        mapHeight(map_height),
        numLayers(num_layers),
        depthMap(std::make_shared<Core::Tex3D>(GL_TEXTURE_CUBE_MAP_ARRAY, GL_DEPTH_COMPONENT24, map_width, map_height, 6 * num_layers, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, true)),
        fbo(map_width, map_height)
    {
        if (!program) {
            program = std::make_shared<Core::Program>("shaders/shaderv_shadowcube.vs", "shaders/shaderf_shadowcube.fs", "shaders/shaderg_shadowcube.gs");
        }
        fbo.Bind();
        fbo.AttachDepthTex(depthMap);
        glDrawBuffer(GL_NONE);  // No colorbuffer
        glReadBuffer(GL_NONE);  // No colorbuffer
        fbo.CheckStatus();
    }

    void PointShadowModule::Render(Scene::Scene& scene, const std::vector<Scene::SceneNode*>& pointlight_nodes) {
        // ---- Generate PointLight shadow maps ----
        fbo.Bind();
        fbo.SetViewportDims();
        fbo.ClearDepth();
        program->Use();

        for (int i = 0, shadow_count = 0; i < pointlight_nodes.size(); i++) {
            Component::PointLight& pointlight = dynamic_cast<Component::PointLight&>(*pointlight_nodes[i]->component);

            setGlobalUniforms(pointlight, pointlight_nodes[i]->transform.position, &shadow_count);
            
            glCullFace(GL_FRONT);
            scene.Draw(*this);
            glCullFace(GL_BACK);
        }
    }

    void PointShadowModule::setGlobalUniforms(Component::PointLight& pl, const glm::vec3& position, int* shadow_idx) {
        if (!pl.HasShadows())
            return;
        program->SetInt("layer", *(shadow_idx)++);
        for (int j = 0; j < 6; j++) {
            program->SetMat4("lightSpaceMatrices[" + std::to_string(j) + "]", pl.LightspaceMatrices()[j]);
        }
        program->SetVec3("lightPos", position);
        program->SetFloat("far", pl.FarPlane());
    }

    void PointShadowModule::SetObjectUniforms(const glm::mat4& model) {
        program->SetMat4("model", model);
    }

    std::shared_ptr<Core::Program> PointShadowModule::GetProgram() {
        return program;
    }

    SkyboxModule::SkyboxModule() {
        if (!program) {
            program = std::make_shared<Core::Program>("shaders/shaderv_skybox.vs", "shaders/shaderf_skybox.fs");
        }
    }
    
    void SkyboxModule::Render(Core::Tex2D& skybox, Component::Camera& camera) {
        static Component::Cube cube(1,1);
        
        program->Use();
        setGlobalUniforms(camera, 0);
        skybox.Bind(0);
        
        glCullFace(GL_FRONT);
        cube.Draw(*this);
        glCullFace(GL_BACK);
    }

    void SkyboxModule::setGlobalUniforms(Component::Camera& camera, int tex_unit) {
        program->SetMat4("view", glm::mat4(glm::mat3(camera.View())));
        program->SetMat4("projection", camera.projection);
        program->SetInt("cubemap", tex_unit);
    }

    std::shared_ptr<Core::Program> SkyboxModule::GetProgram() {
        return program;
    }

    SsaoModule::SsaoModule()
        : ssao(std::make_shared<Core::Tex2D>(GL_TEXTURE_2D, GL_RED, 2, 2, GL_RED, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR)),
        ssaoBlurred(std::make_shared<Core::Tex2D>(GL_TEXTURE_2D, GL_RED, 2, 2, GL_RED, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR)),
        noise(GL_TEXTURE_2D, GL_RGBA16F, 4, 4, GL_RGB, GL_FLOAT, GL_REPEAT, GL_NEAREST),
        fbo(2,2)
    {
        if (!program) {
            program = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dssao.fs");
            programBlur = std::make_shared<Core::Program>("shaders/shaderv_2d.vs", "shaders/shaderf_2dssaoblur.fs");
        }
        fbo.Bind();
        fbo.AttachColorTex(ssao);
        fbo.AttachColorTex(ssaoBlurred);
        fbo.CheckStatus();

        // Generate sampling kernel (random vectors in tangent space in the +normal hemisphere)
        std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
        std::default_random_engine generator;
        for (int i = 0; i < 64; i++) {
            glm::vec3 sample(
                randomFloats(generator) * 2.0f - 1.0f,
                randomFloats(generator) * 2.0f - 1.0f,
                randomFloats(generator)
            );
            sample = randomFloats(generator) * glm::normalize(sample);
            float scale = (float)i / 64.0f;
            scale = 0.1f + scale*scale * (1.0f - 0.1f);    // lerp, places more samples closer to origin of hemisphere
            sample *= scale;
            kernel.push_back(sample);
        }

        // Generate noise texture (random vectors in tangent space on the tangent-bitangent plane)
        std::vector<glm::vec3> noiseVector;    
        for (int i = 0; i < 16; i++) {
            glm::vec3 sample(
                randomFloats(generator) * 2.0f - 1.0f,
                randomFloats(generator) * 2.0f - 1.0f, 
                0.0f
            );
            noiseVector.push_back(sample);
        }
        noise.Bind();
        noise.DefineImage(noiseVector.data());
    }

    void SsaoModule::Render(Core::Tex& positions, Core::Tex& normals, const glm::mat4& projection) {
        if (positions.width != fbo.width || positions.height != fbo.height) {
            fbo.Resize(positions.width, positions.height);
        }

        fbo.Bind();
        fbo.SetViewportDims();

        // SSAO unblurred
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        fbo.ClearColor();
        
        program->Use();
        program->SetInt("gPosition", 0);
        program->SetInt("gNormal", 1);
        program->SetInt("noiseTexture", 2);
        program->SetMat4("projection", projection);
        positions.Bind(0);
        normals.Bind(1);
        noise.Bind(2);
        
        for (int i = 0; i < kernel.size(); i++) {
            program->SetVec3("samples[" + std::to_string(i) + "]", kernel[i]);
        }
        Component::Primitive::DrawQuad();
        
        // Blurring
        glDrawBuffer(GL_COLOR_ATTACHMENT1);
        fbo.ClearColor(1,1,1,1);
        
        programBlur->Use();
        programBlur->SetInt("ssaoInput", 0);
        fbo.colorAtts[0]->Bind();
        Component::Primitive::DrawQuad();
    }

    std::shared_ptr<Core::Program> SsaoModule::GetProgram() {
        return program;
    }

}