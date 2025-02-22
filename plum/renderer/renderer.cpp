#include <plum/renderer/renderer.hpp>
#include <iostream>

namespace Renderer {

    BaseRenderer::BaseRenderer() {}
    BaseRenderer::~BaseRenderer() {}

    DeferredRenderer::DeferredRenderer() : gBuffer(1920,1080), dirShadowBuffer(2048,2048), pointShadowBuffer(1024,1024), output(1920,1080) {
        InitGbuffer();
        InitShadowMaps();
        InitializeUniformBlocks();
    }
    DeferredRenderer::~DeferredRenderer() {}
        
    Component::Fbo DeferredRenderer::Render(Component::Scene& scene, Camera& camera) {
        // SetUniforms(scene, camera); // global uniforms
        GeometryPass();
        ShadowMapPass();
        LightingPass();
        ForwardPass();
        return output;
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
    }
    
    void DeferredRenderer::SetUniforms(Component::Scene& scene, Camera& camera) {
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
        float pointlight_count = 0, pointlight_shadow_count = 0;
        for (auto& node : scene.root.children) {
            if (!node->object->IsLight())
                continue;

            if (node->object->objType == Component::SceneObject::SceneObjectType::DirLight) {
                auto dl = dynamic_cast<Component::DirectionalLight*>(node->object.get());
                if (dl)
                    SetDirectionalLightUniforms(*dl, camera);
            }
            else if (node->object->objType == Component::SceneObject::SceneObjectType::PointLight) {
                auto pl = dynamic_cast<Component::PointLight*>(node->object.get());
                if (pl)
                    SetPointLightUniforms(*pl, camera);
            }
        }
    }

    void DeferredRenderer::SetDirectionalLightUniforms(Component::DirectionalLight& dl, Camera& camera) {
        float count = 0;
        int shadow_count = 0;

        for (int i = 0; i < count; i++) {
            unsigned int offset = 16 + i*96;
            
            glm::vec4 dirlight_color = glm::vec4(dl.color, 0);
            uboFsDirlight->UpdateData(offset, sizeof(glm::vec4), &dirlight_color);
            
            if (dl.HasShadows()) {
                glm::vec4 dirlight_direction = 
                glm::vec4( glm::mat3(glm::transpose(glm::inverse(camera.View))) * dl.direction, shadow_count++);
                uboFsDirlight->UpdateData(offset + 16, sizeof(glm::vec4), &dirlight_direction);
                
                glm::mat4 dirlight_matrix = dl.GetLightspaceMatrix();
                uboFsDirlight->UpdateData(offset + 32, sizeof(glm::mat4), &dirlight_matrix);
            } else {
                glm::vec4 dirlight_direction = 
                glm::vec4( glm::mat3(glm::transpose(glm::inverse(camera.View))) * dl.direction, -1 );
                uboFsDirlight->UpdateData(offset + 16, sizeof(glm::vec4), &dirlight_direction);
                
                // Don't need to set lightspace matrix if not casting shadows
            }
            
        }
        uboFsDirlight->UpdateData(0, sizeof(float), &count);
    }

    void DeferredRenderer::SetPointLightUniforms(Component::PointLight& pl, Camera& camera) {
        float count = 0;
        int shadow_count = 0;

        for (int i = 0; i < count; i++) {
            unsigned int offset = 16 + i*64;

            glm::vec4 pointlight_color = glm::vec4(pl.color, 0);
            uboFsPointlight->UpdateData(offset, sizeof(glm::vec4), &pointlight_color);

            float pointlight_att[] = { pl.GetAttenuationConstant(), pl.GetAttenuationLinear(), pl.GetAttenuationQuadratic() };
            glm::vec4 att = glm::vec4(pointlight_att[0], pointlight_att[1], pointlight_att[2], 0);
            uboFsPointlight->UpdateData(offset + 16, sizeof(glm::vec4), &att);

            glm::vec4 pointlight_pos_view = camera.View * glm::vec4(pl.position, 1);
            pointlight_pos_view.w = pl.GetFarPlane();
            uboFsPointlight->UpdateData(offset + 32, sizeof(glm::vec4), &pointlight_pos_view);

            glm::vec4 pointlight_pos_world = glm::vec4(pl.position, -1);
            if (pl.HasShadows()) {
                pointlight_pos_world = glm::vec4(pl.position, shadow_count++);
            }
            uboFsPointlight->UpdateData(offset + 48, sizeof(glm::vec4), &pointlight_pos_world);
        }

        uboFsPointlight->UpdateData(0, sizeof(float), &count);
    }

    void DeferredRenderer::InitGbuffer() {
        
        using namespace Component;
        
        gBuffer.Bind();
        
        Texture position = Texture::Texture2D(GL_TEXTURE_2D, GL_RGBA32F, 1920, 1080, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST);
        gBuffer.AttachColorTexture(position);

        Texture normal = Texture::Texture2D(GL_TEXTURE_2D, GL_RGBA32F, 1920, 1080, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST);
        gBuffer.AttachColorTexture(normal);
        
        Texture albedospec = Texture::Texture2D(GL_TEXTURE_2D, GL_RGBA, 1920, 1080, GL_RGBA, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_NEAREST);
        gBuffer.AttachColorTexture(albedospec);
        
        Texture metrou = Texture::Texture2D(GL_TEXTURE_2D, GL_RGBA, 1920, 1080, GL_RGBA, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_NEAREST);
        gBuffer.AttachColorTexture(albedospec);
        
        gBuffer.AttachDepthRbo16();
    
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "framebuffer error: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
            exit(1);
        }
    }

    void DeferredRenderer::InitShadowMaps() {
        
        using namespace Component;

        GLsizei nlayers_2d      = 8;        
        GLsizei nlayers_cube    = 6 * 8;

        // Shadow map array for directional lights
        dirShadowBuffer.Bind();
        
        Texture dirShadowTex = Texture::Texture3D(GL_TEXTURE_2D_ARRAY, GL_DEPTH_COMPONENT16, dirShadowBuffer.width, dirShadowBuffer.height, nlayers_2d, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR);
        dirShadowTex.Bind();
        
        float border_color[] = {1.f, 1.f, 1.f, 1.f};
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, border_color);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        dirShadowBuffer.AttachDepthTexture(dirShadowTex);
        glDrawBuffer(GL_NONE);  // No colorbuffer
        glReadBuffer(GL_NONE);  // No colorbuffer
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Framebuffer error: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
        }
        
        // Shadow map array for point lights
        pointShadowBuffer.Bind();

        Texture pointShadowTex = Texture::Texture3D(GL_TEXTURE_CUBE_MAP_ARRAY, GL_DEPTH_COMPONENT16, pointShadowBuffer.width, pointShadowBuffer.height, nlayers_cube, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR);
        pointShadowTex.Bind();
        
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        pointShadowBuffer.AttachDepthTexture(dirShadowTex);
        glDrawBuffer(GL_NONE);  // No colorbuffer
        glReadBuffer(GL_NONE);  // No colorbuffer
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Framebuffer error: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
        }
    }

    void DeferredRenderer::GeometryPass() {
        gBuffer.Bind();
        gBuffer.SetViewportDims();
        gBuffer.ClearColor();
        gBuffer.ClearDepth();
        // draw...
    }

    void DeferredRenderer::ShadowMapPass() {
        dirShadowBuffer.Bind();
        dirShadowBuffer.SetViewportDims();
        dirShadowBuffer.ClearDepth();
        // draw...
            // use shadow shader program

        pointShadowBuffer.Bind();
        pointShadowBuffer.SetViewportDims();
        pointShadowBuffer.ClearDepth();
        // draw...
            // use shadow shader program
    }

    void DeferredRenderer::LightingPass() {
        output.Bind();
        output.SetViewportDims();
        output.ClearColor();
        output.ClearDepth();

        // draw...
            // use pbr program
            // gbuffer textures and uniforms
            // pbr ibl textures and uniforms
            // shadow map textures and uniforms
    }

    void DeferredRenderer::ForwardPass() {
        output.Bind();
        output.SetViewportDims();

        // Blit depth information from gBuffer to output
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.Handle());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, output.Handle());
        glBlitFramebuffer(0, 0, gBuffer.width, gBuffer.height, 0, 0, output.width, output.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        
        // draw...
            // anything bypassing lighting calcs
            // skybox
    }
}