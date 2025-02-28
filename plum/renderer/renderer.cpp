#include <plum/renderer/renderer.hpp>
#include <plum/component/primitive.hpp>
#include <iostream>
#include <queue>

namespace Renderer {

    BaseRenderer::BaseRenderer() {}
    BaseRenderer::~BaseRenderer() {}

    DeferredRenderer::DeferredRenderer() : gBuffer(1920,1080), dirShadowBuffer(2048,2048), pointShadowBuffer(1024,1024), output(1920,1080) {
        InitializeUniformBlocks();
        InitGbuffer();
        InitShadowMaps();
    }
    DeferredRenderer::~DeferredRenderer() {}
        
    Component::Fbo& DeferredRenderer::Render(Component::Scene& scene, Material::Environment& env, Camera& camera) {
        ParseScene(scene);
        UpdateUniformBuffers(scene, camera);
        GeometryPass(scene, camera);
        ShadowMapPass(scene);
        LightingPass(scene, env, camera);
        ForwardPass(scene, env, camera);
        return output;
        // some postprocessing like Bloom requires additional render info
        // need to support intraprocessing rendering options
            // - Bloom
            // - SSAO
            // ....
    }
    
    void DeferredRenderer::InitializeUniformBlocks() {
        // 0 - View, projection transforms  
        uboVsMatrices   = std::make_shared<Component::Ubo>(0, 2*sizeof(glm::mat4));
        // 1 - Inverse view transform
        uboFsMatrices   = std::make_shared<Component::Ubo>(1, 1*sizeof(glm::mat4));
        // 2 - Camera position, front (viewspace)
        uboFsCamera     = std::make_shared<Component::Ubo>(2, 2*16);
        // 3 - Directional light colors, direction, lightspace transform
        uboFsDirlight   = std::make_shared<Component::Ubo>(3, 16 + 8 * (16+16 + sizeof(glm::mat4)));
        // 4 - Point light count, colors, attenuations, positions, positions (worldspace)
        uboFsPointlight = std::make_shared<Component::Ubo>(4, 16 + 32 * (4*16));

        // Set UBO scheme to default (may implement non-default schemes in the future)
        lightingPassPbrModule.GetProgram()->SetUniformBlockBindingScheme(Material::Program::UboScheme::Scheme1);
    }

    void DeferredRenderer::InitGbuffer() {
        
        using namespace Component;
        
        gBuffer.Bind();
        
        Tex2D position = Tex2D(GL_TEXTURE_2D, GL_RGBA32F, gBuffer.width, gBuffer.height, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST);
        gBuffer.AttachColorTexture(position);

        Tex2D normal = Tex2D(GL_TEXTURE_2D, GL_RGBA32F, gBuffer.width, gBuffer.height, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST);
        gBuffer.AttachColorTexture(normal);
        
        Tex2D albedospec = Tex2D(GL_TEXTURE_2D, GL_RGBA, gBuffer.width, gBuffer.height, GL_RGBA, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_NEAREST);
        gBuffer.AttachColorTexture(albedospec);
        
        Tex2D metrou = Tex2D(GL_TEXTURE_2D, GL_RGBA, gBuffer.width, gBuffer.height, GL_RGBA, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_NEAREST);
        gBuffer.AttachColorTexture(albedospec);
        
        gBuffer.AttachDepthRbo16();
    
        gBuffer.CheckStatus();
    }

    void DeferredRenderer::InitShadowMaps() {
        
        using namespace Component;

        GLsizei nlayers_2d      = 8;        
        GLsizei nlayers_cube    = 6 * 8;

        // Shadow map array for DirectionalLight
        Tex3D dirShadowTex = Tex3D(GL_TEXTURE_2D_ARRAY, GL_DEPTH_COMPONENT16, dirShadowBuffer.width, dirShadowBuffer.height, nlayers_2d, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, true);
        
        dirShadowBuffer.Bind();
        dirShadowBuffer.AttachDepthTexture(dirShadowTex);
        glDrawBuffer(GL_NONE);  // No colorbuffer
        glReadBuffer(GL_NONE);  // No colorbuffer
        dirShadowBuffer.CheckStatus();
        
        // Shadow map array for PointLight
        Tex3D pointShadowTex = Tex3D(GL_TEXTURE_CUBE_MAP_ARRAY, GL_DEPTH_COMPONENT16, pointShadowBuffer.width, pointShadowBuffer.height, nlayers_cube, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, true);
        
        pointShadowBuffer.Bind();
        pointShadowBuffer.AttachDepthTexture(dirShadowTex);
        glDrawBuffer(GL_NONE);  // No colorbuffer
        glReadBuffer(GL_NONE);  // No colorbuffer
        pointShadowBuffer.CheckStatus();
    }

    void DeferredRenderer::ParseScene(Component::Scene& scene) {
        
        using namespace Component;

        // ---- Clear existing lists ----
        directionalLights.clear();
        pointLights.clear();
        
        // ---- Parse scene for Light nodes ----
        std::queue<SceneNode*> node_queue;

        node_queue.push(&scene.root);
        while (!node_queue.empty()) {
            SceneNode* curr = node_queue.front();
            node_queue.pop();

            if (curr->object->IsLight()) {
                if (curr->object->objType == SceneObject::SceneObjectType::DirLight)
                    directionalLights.push_back(curr);
                else if (curr->object->objType == SceneObject::SceneObjectType::PointLight)
                    pointLights.push_back(curr); 
            }

            for (auto& child : curr->children)
                node_queue.push(child.get());
        }
    }

    void DeferredRenderer::UpdateUniformBuffers(Component::Scene& scene, Camera& camera) {
        // 0 - View, projection transforms  
        uboVsMatrices->UpdateData(0, sizeof(glm::mat4), &camera.View);
        uboVsMatrices->UpdateData(sizeof(glm::mat4), sizeof(glm::mat4), &camera.Projection);
        // 1 - Inverse view transform
        glm::mat4 inv_view = glm::inverse(camera.View);
        uboFsMatrices->UpdateData(0, sizeof(glm::mat4), &inv_view);
        // 2 - Camera position, front (viewspace)
        glm::vec3 dummy_cam_pos = glm::vec3(0); // shaders work in view space
        uboFsCamera->UpdateData(0, sizeof(glm::vec3), &dummy_cam_pos);
        uboFsCamera->UpdateData(16, sizeof(glm::vec3), &camera.Front);

        // 3 - Directional light colors, direction, lightspace transform
        // 4 - Point light count, colors, attenuations, positions, positions (worldspace)
        SetDirectionalLightUniforms(camera);
        SetPointLightUniforms(camera);
    }

    void DeferredRenderer::SetDirectionalLightUniforms(Camera& camera) {
        int total_count = directionalLights.size();
        int shadow_count = 0;
        
        uboFsDirlight->UpdateData(0, sizeof(float), &total_count);

        for (int i = 0; i < total_count; i++) {
            Component::DirectionalLight* dl = dynamic_cast<Component::DirectionalLight*>(directionalLights[i]->object.get());

            unsigned int offset = 16 + i*96;
            
            glm::vec4 dirlight_color = glm::vec4(dl->color, 0);
            uboFsDirlight->UpdateData(offset, sizeof(glm::vec4), &dirlight_color);
            
            if (dl->HasShadows()) {
                glm::vec4 dirlight_direction = glm::vec4( glm::mat3(glm::transpose(glm::inverse(camera.View))) * dl->direction, shadow_count++);
                uboFsDirlight->UpdateData(offset + 16, sizeof(glm::vec4), &dirlight_direction);
                
                glm::mat4 dirlight_matrix = dl->GetLightspaceMatrix();
                uboFsDirlight->UpdateData(offset + 32, sizeof(glm::mat4), &dirlight_matrix);
            } else {
                glm::vec4 dirlight_direction = glm::vec4( glm::mat3(glm::transpose(glm::inverse(camera.View))) * dl->direction, -1 );
                uboFsDirlight->UpdateData(offset + 16, sizeof(glm::vec4), &dirlight_direction);
                
                // (Don't need to set lightspace matrix if not casting shadows)
            }
            
        }
        int total_count = directionalLights.size();
    }

    void DeferredRenderer::SetPointLightUniforms(Camera& camera) {
        int total_count = pointLights.size();
        int shadow_count = 0;

        uboFsPointlight->UpdateData(0, sizeof(float), &total_count);

        for (int i = 0; i < total_count; i++) {
            Component::PointLight* pl = dynamic_cast<Component::PointLight*>(pointLights[i]->object.get());

            unsigned int offset = 16 + i*64;

            glm::vec4 pointlight_color = glm::vec4(pl->color, 0);
            uboFsPointlight->UpdateData(offset, sizeof(glm::vec4), &pointlight_color);

            float pointlight_att[] = { pl->GetAttenuationConstant(), pl->GetAttenuationLinear(), pl->GetAttenuationQuadratic() };
            glm::vec4 att = glm::vec4(pointlight_att[0], pointlight_att[1], pointlight_att[2], 0);
            uboFsPointlight->UpdateData(offset + 16, sizeof(glm::vec4), &att);

            glm::vec4 pointlight_pos_view = camera.View * glm::vec4(pl->position, 1);
            pointlight_pos_view.w = pl->GetFarPlane();
            uboFsPointlight->UpdateData(offset + 32, sizeof(glm::vec4), &pointlight_pos_view);

            glm::vec4 pointlight_pos_world = glm::vec4(pl->position, -1);
            if (pl->HasShadows()) {
                pointlight_pos_world = glm::vec4(pl->position, shadow_count++);
            }
            uboFsPointlight->UpdateData(offset + 48, sizeof(glm::vec4), &pointlight_pos_world);
        }
    }

    void DeferredRenderer::GeometryPass(Component::Scene& scene, Camera& camera) {
        gBuffer.Bind();
        gBuffer.SetViewportDims();
        gBuffer.ClearColor();
        gBuffer.ClearDepth();
        // need logic for multiple materials - use visibility buffer later, use branching now
        scene.Draw();
    }

    void DeferredRenderer::ShadowMapPass(Component::Scene& scene) {        
        // ---- Generate DirectionalLight shadow maps ----
        dirShadowBuffer.Bind();
        dirShadowBuffer.SetViewportDims();
        dirShadowBuffer.ClearDepth();
        dirShadowModule.Use();
        
        for (int i = 0, shadow_count = 0; i < directionalLights.size(); i++) {
            Component::DirectionalLight* dl = dynamic_cast<Component::DirectionalLight*>(directionalLights[i]->object.get());
            
            dirShadowModule.SetGlobalUniforms(*dl, dirShadowBuffer.depthAtt->Handle(), &shadow_count);
            
            glCullFace(GL_FRONT);
            scene.Draw(dirShadowModule);
            glCullFace(GL_BACK);
        }
        
        // ---- Generate PointLight shadow maps ----
        pointShadowBuffer.Bind();
        pointShadowBuffer.SetViewportDims();
        pointShadowBuffer.ClearDepth();
        pointShadowModule.Use();

        for (int i = 0, shadow_count = 0; i < pointLights.size(); i++) {
            Component::PointLight* pl = dynamic_cast<Component::PointLight*>(pointLights[i]->object.get());

            pointShadowModule.SetGlobalUniforms(*pl, pointLights[i]->transform.position, &shadow_count);
            
            glCullFace(GL_FRONT);
            scene.Draw(pointShadowModule);
            glCullFace(GL_BACK);
        }
    }

    void DeferredRenderer::LightingPass(Component::Scene& scene, Material::Environment& env, Camera& camera) {
        // ---- Prep framebuffer ----
        output.Bind();
        output.SetViewportDims();
        output.ClearColor();
        output.ClearDepth();
        
        // ---- Bind textures ---- 
        // G-buffer
        for (int i = 0; i < gBuffer.colorAtts.size(); i++)
            gBuffer.colorAtts[i]->Bind(i);
        // SSAO
            // (future)
        // IBL
        env.irradiance->Bind(5);
        env.prefilter->Bind(6);
        env.brdfLut->Bind(7);
        // Shadow maps
        dirShadowBuffer.depthAtt->Bind(8);
        pointShadowBuffer.depthAtt->Bind(9);
        
        // ---- Set uniforms ---- 
        lightingPassPbrModule.gPosition = 1;
        lightingPassPbrModule.gNormal = 2;
        lightingPassPbrModule.gAlbedoSpec = 3;
        lightingPassPbrModule.gMetRouOcc = 4;
        lightingPassPbrModule.irradianceMap = 5;
        lightingPassPbrModule.prefilterMap = 6;
        lightingPassPbrModule.brdfLUT = 7;
        lightingPassPbrModule.ibl = 0.5f;
        lightingPassPbrModule.shadowmap_2d_array_shadow = 8;
        lightingPassPbrModule.shadowmap_cube_array_shadow = 9;
        lightingPassPbrModule.Use();
        lightingPassPbrModule.SetGlobalUniforms();
        
        // ---- Draw ----
        Component::Primitive::DrawQuad();
    }

    void DeferredRenderer::ForwardPass(Component::Scene& scene, Material::Environment& env, Camera& camera) {
        // ---- Blit depth data from gBuffer to output ----
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.Handle());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, output.Handle());
        glBlitFramebuffer(0, 0, gBuffer.width, gBuffer.height, 0, 0, output.width, output.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        
        // ---- Prep framebuffer ----
        output.Bind();
        output.SetViewportDims();

        // draw...
            // anything bypassing lighting calcs
        
        // ---- Draw skybox ----
        static Component::Cube cube;
        env.skyboxModule.Use();
        env.skyboxModule.SetGlobalUniforms(glm::mat4(glm::mat3(camera.View)), camera.Projection, 0);
        env.skybox->Bind(0);
        glCullFace(GL_FRONT);
        cube.Draw(env.skyboxModule);
        glCullFace(GL_BACK);
    }
}