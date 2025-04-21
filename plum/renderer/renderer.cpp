#include <plum/renderer/renderer.hpp>

#include <plum/component/primitive.hpp>
#include <plum/context/application.hpp>
#include <plum/asset/manager.hpp>
#include <plum/material/texture.hpp>

#include <functional>
#include <iostream>
#include <queue>

namespace Renderer {

    RendererBase::RendererBase() 
        : window(Context::Application::Instance().activeWindow)
    {
        EnableDepth();
        EnableCull();
        // EnableFramebufferSrgb();
        EnableSeamlessCubeMap();
    }
    
    void RendererBase::EnableDepth(GLenum func) {
        // Default LEQUAL for skybox optimization, since depth buffer is cleared to 1.0 by default
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(func);
    }
    void RendererBase::EnableCull(GLenum mode) {
        glEnable(GL_CULL_FACE);
        glCullFace(mode);
    }
    void RendererBase::EnableFramebufferSrgb() {
        glEnable(GL_FRAMEBUFFER_SRGB);
    }
    void RendererBase::EnableSeamlessCubeMap() {
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // Doesn't change
    }

    // void RendererBase::DisableDepth() {
    //     glDisable(GL_DEPTH_TEST);
    // }
    // void RendererBase::DisableCull() {
    //     glDisable(GL_CULL_FACE);
    // }
    // void RendererBase::DisableFramebufferSrgb() {
    //     glDisable(GL_FRAMEBUFFER_SRGB);
    // }
    
    // void ClearColor() {}
    // void ClearDepth() {}
    // void ClearStencil() {}

    DeferredRenderer::DeferredRenderer() 
        : gBuffer(window->Width(), window->Height()), 
        output(window->Width(), window->Height()),
        dirShadowModule(1024, 1024),
        pointShadowModule(1024, 1024),
        eventListener(Context::InputsAndEventsManager::CreateEventListener())
    {
        if (!lightingPassProgram) {
            AssetManager& manager = AssetManager::Instance();
            auto vs = manager.LoadHot<ShaderAsset>("shaders/shaderv_2d.vs", GL_VERTEX_SHADER);
            auto fs = manager.LoadHot<ShaderAsset>("shaders/shaderf_lightingpasspbr.fs", GL_FRAGMENT_SHADER);
            
            lightingPassProgram = std::make_shared<Core::Program>(vs, fs);
            lightingPassProgram->SetUniformBlockBindingScheme(Core::Program::UboScheme::Scheme1);
        }

        initUniformBlocks();
        initGbuffer();
        initOutput();

        std::function<void(int,int)> staticFunc = std::bind(&DeferredRenderer::framebufferSizeCallback, this, std::placeholders::_1, std::placeholders::_2);
        eventListener->SetFramebufferSizeCallback(staticFunc);
    }

    DeferredRenderer::~DeferredRenderer() {}
    
    Core::Fbo* DeferredRenderer::Render(Scene::Scene& scene, Component::Camera& camera) {
        static Scene::Environment environment;
        return Render(scene, camera, environment);
    }

    Core::Fbo* DeferredRenderer::Render(Scene::Scene& scene, Component::Camera& camera, Scene::Environment& env) {
        updateGlobalUniforms(scene, camera);
        geometryPass(scene);
        if (ssao)
            ssaoPass(camera);
        shadowMapPass(scene);
        lightingPass(env);
        forwardPass(camera, env);
        return &output;
        // some postprocessing like Bloom requires additional render info
        // need to support intraprocessing rendering options
            // - SSAO
            // ....
    }
    
    void DeferredRenderer::initUniformBlocks() {
        // Core::Program::UboScheme::Scheme1
        // Should have some help from Program to enforce this sceheme
        // 0 - View, projection transforms  
        uboVsMatrices   = std::make_shared<Core::Ubo>(0, 2*sizeof(glm::mat4));
        // 1 - Inverse view transform
        uboFsMatrices   = std::make_shared<Core::Ubo>(1, 1*sizeof(glm::mat4));
        // 2 - Camera position, front (viewspace)
        uboFsCamera     = std::make_shared<Core::Ubo>(2, 2*16);
        // 3 - Directional light colors, direction, lightspace transform
        uboFsDirlight   = std::make_shared<Core::Ubo>(3, 16 + 8 * (16+16 + sizeof(glm::mat4)));
        // 4 - Point light count, colors, attenuations, positions, positions (worldspace)
        uboFsPointlight = std::make_shared<Core::Ubo>(4, 16 + 32 * (4*16));

        // Set UBO scheme to default (may implement non-default schemes in the future)
        lightingPassProgram->SetUniformBlockBindingScheme(Core::Program::UboScheme::Scheme1);
    }

    void DeferredRenderer::initGbuffer() {
        gBuffer.Bind();
        gBuffer.colorAtts.resize(4);
        
        auto position = std::make_shared<Core::Tex2D>(GL_TEXTURE_2D, GL_RGBA32F, gBuffer.width, gBuffer.height, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST);
        gBuffer.AttachColorTex(position, 0);

        auto normal = std::make_shared<Core::Tex2D>(GL_TEXTURE_2D, GL_RGBA32F, gBuffer.width, gBuffer.height, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST);
        gBuffer.AttachColorTex(normal, 1);
        
        auto albedospec = std::make_shared<Core::Tex2D>(GL_TEXTURE_2D, GL_RGBA32F, gBuffer.width, gBuffer.height, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST);
        gBuffer.AttachColorTex(albedospec, 2);
        
        auto metrou = std::make_shared<Core::Tex2D>(GL_TEXTURE_2D, GL_RGBA, gBuffer.width, gBuffer.height, GL_RGBA, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_NEAREST);
        gBuffer.AttachColorTex(metrou, 3);
        
        gBuffer.AttachDepthRbo24();
    
        gBuffer.CheckStatus();
    }

    void DeferredRenderer::initOutput() {\
        auto color = std::make_shared<Core::Tex2D>(GL_TEXTURE_2D, GL_RGBA32F, output.width, output.height, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR);

        output.Bind();
        output.AttachColorTex(color);
        output.AttachDepthRbo24();
        output.CheckStatus();
    }

    void DeferredRenderer::parseLights(Scene::Scene& scene) {
        // ---- Clear existing lists ----
        directionalLightNodes.clear();
        pointLightNodes.clear();
        
        // ---- Parse scene for Light nodes ----
        std::queue<Scene::SceneNode*> node_queue;

        node_queue.emplace(&scene);
        while (!node_queue.empty()) {
            Scene::SceneNode* curr = node_queue.front();
            node_queue.pop();

            if (curr->component) {
                if (curr->component->IsLight()) {
                    if (curr->component->type == Component::ComponentType::DirLight)
                        directionalLightNodes.emplace_back(curr);
                    else if (curr->component->type == Component::ComponentType::PointLight)
                        pointLightNodes.emplace_back(curr); 
                }
            }

            for (const auto& child : curr->children)
                node_queue.emplace(child.get());
        }
    }

    void DeferredRenderer::updateGlobalUniforms(Scene::Scene& scene, Component::Camera& camera) {
        // 0 - View, projection transforms  
        uboVsMatrices->UpdateData(0, sizeof(glm::mat4), &camera.View());
        uboVsMatrices->UpdateData(sizeof(glm::mat4), sizeof(glm::mat4), &camera.projection);
        // 1 - Inverse view transform
        glm::mat4 inv_view = glm::inverse(camera.View());
        uboFsMatrices->UpdateData(0, sizeof(glm::mat4), &inv_view);
        // 2 - Camera position, front (viewspace)
        const glm::vec3 dummy_cam_pos = glm::vec3(0); // shaders work in view space
        uboFsCamera->UpdateData(0, sizeof(glm::vec3), &dummy_cam_pos);
        uboFsCamera->UpdateData(16, sizeof(glm::vec3), &camera.transform.Front());

        // 3 - Directional light colors, direction, lightspace transform
        // 4 - Point light count, colors, attenuations, positions, positions (worldspace)
        parseLights(scene);
        setDirectionalLightUniforms(camera);
        setPointLightUniforms(camera);
    }

    void DeferredRenderer::setDirectionalLightUniforms(Component::Camera& camera) {
        float total_count = directionalLightNodes.size();
        int shadow_count = 0;
        
        uboFsDirlight->UpdateData(0, sizeof(float), &total_count);

        for (int i = 0; i < total_count; i++) {
            Component::DirectionalLight& dirlight = dynamic_cast<Component::DirectionalLight&>(*directionalLightNodes[i]->component);

            unsigned int offset = 16 + i*96;
            
            // Color
            glm::vec4 color = glm::vec4(dirlight.color * dirlight.intensity, 0);
            uboFsDirlight->UpdateData(offset, sizeof(glm::vec4), &color);
            
            // Direction and shadow map index
            glm::vec4 direction = glm::vec4( glm::mat3(glm::transpose(glm::inverse(camera.View()))) * directionalLightNodes[i]->transform.Front(), -1 );
            if (dirlight.HasShadows()) {
                direction = glm::vec4( glm::mat3(glm::transpose(glm::inverse(camera.View()))) * directionalLightNodes[i]->transform.Front(), shadow_count++ );
                uboFsDirlight->UpdateData(offset + 32, sizeof(glm::mat4), &dirlight.LightspaceMatrix());
            }
            uboFsDirlight->UpdateData(offset + 16, sizeof(glm::vec4), &direction);
        }
    }

    void DeferredRenderer::setPointLightUniforms(Component::Camera& camera) {
        float total_count = pointLightNodes.size();
        int shadow_count = 0;

        uboFsPointlight->UpdateData(0, sizeof(float), &total_count);

        for (int i = 0; i < total_count; i++) {
            Component::PointLight& pointlight = dynamic_cast<Component::PointLight&>(*pointLightNodes[i]->component);

            unsigned int offset = 16 + i*64;
            
            // Color
            glm::vec4 color = glm::vec4(pointlight.color * pointlight.intensity, 0);
            uboFsPointlight->UpdateData(offset, sizeof(glm::vec4), &color);

            // Attenuation
            glm::vec4 attenuation = glm::vec4(pointlight.AttenuationConstant(), pointlight.AttenuationLinear(), pointlight.AttenuationQuadratic(), 0);
            uboFsPointlight->UpdateData(offset + 16, sizeof(glm::vec4), &attenuation);
            
            // Position (viewspace)
            glm::vec4 position_view = camera.View() * glm::vec4(pointLightNodes[i]->transform.position, 1);
            position_view.w = pointlight.FarPlane();
            uboFsPointlight->UpdateData(offset + 32, sizeof(glm::vec4), &position_view);
            // Position (worldspace) and shadow map index
            glm::vec4 position_world = glm::vec4(pointLightNodes[i]->transform.position, -1);
            if (pointlight.HasShadows()) {
                position_world = glm::vec4(pointLightNodes[i]->transform.position, shadow_count++);
            }
            uboFsPointlight->UpdateData(offset + 48, sizeof(glm::vec4), &position_world);
        }
    }

    void DeferredRenderer::geometryPass(Scene::Scene& scene) {
        gBuffer.Bind();
        gBuffer.SetViewportDims();
        gBuffer.ClearColor();
        gBuffer.ClearDepth();
        // need logic for multiple materials - use visibility buffer later, use branching now
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        scene.Draw();
    }

    void DeferredRenderer::ssaoPass(Component::Camera& camera) {
        ssaoModule.Render(*gBuffer.colorAtts[0], *gBuffer.colorAtts[1], camera.projection);
    }

    void DeferredRenderer::shadowMapPass(Scene::Scene& scene) {        
        dirShadowModule.Render(scene, directionalLightNodes);
        pointShadowModule.Render(scene, pointLightNodes);
    }

    void DeferredRenderer::lightingPass(Scene::Environment& env) {
        // ---- Prep framebuffer ----
        output.Bind();
        output.SetViewportDims();
        output.ClearColor();
        output.ClearDepth();
        
        // ---- Set uniforms ---- 
        lightingPassProgram->Use();
        lightingPassProgram->SetInt("gPosition", 0);
        lightingPassProgram->SetInt("gNormal", 1);
        lightingPassProgram->SetInt("gAlbedoSpec", 2);
        lightingPassProgram->SetInt("gMetRouOcc", 3);
        lightingPassProgram->SetInt("ssao", ssao);
        lightingPassProgram->SetInt("ssaoMap", 4);
        lightingPassProgram->SetInt("irradianceMap", 5);
        lightingPassProgram->SetInt("prefilterMap", 6);
        lightingPassProgram->SetInt("brdfLUT", 7);
        lightingPassProgram->SetInt("shadowmap_2d_array_shadow", 8);
        lightingPassProgram->SetInt("shadowmap_cube_array_shadow", 9);

        // ---- Bind textures ---- 
        // G-buffer
        for (int i = 0; i < gBuffer.colorAtts.size(); i++) {
            gBuffer.colorAtts[i]->Bind(i);
        }
        // SSAO
        if (ssao) {
            ssaoModule.ssao->Bind(4);
        }
        // IBL
        if (env.skybox) {
            env.irradiance->Bind(5);
            env.prefilter->Bind(6);
            env.brdfLut->Bind(7);
            lightingPassProgram->SetFloat("ibl", env.iblIntensity);
        }
        // Shadow maps
        dirShadowModule.depthMap->Bind(8);
        pointShadowModule.depthMap->Bind(9);
        
        // ---- Draw ----
        Component::Primitive::DrawQuad();
    }

    void DeferredRenderer::forwardPass(Component::Camera& camera, Scene::Environment& env) {
        // ---- Blit depth data from gBuffer to output ----
        gBuffer.BlitTo(output, false, true);
        
        // ---- Prep framebuffer ----
        output.Bind();
        output.SetViewportDims();

        // ---- Draw ----
        env.DrawSkybox(camera);
    }

    void DeferredRenderer::framebufferSizeCallback(int width, int height) {
        gBuffer.Resize(width, height);
        output.Resize(width, height);
    }
}
