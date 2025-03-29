#include <plum/material/module.hpp>

#include <plum/component/camera.hpp>
#include <plum/component/light.hpp>
#include <plum/scene/scene.hpp>
#include <plum/scene/scenenode.hpp>

namespace Material {

    DirectionalShadowModule::DirectionalShadowModule(int map_width, int map_height, int num_layers)
        : mapWidth(map_width),
        mapHeight(map_height),
        numLayers(num_layers),
        depthMap(std::make_shared<Core::Tex3D>(GL_TEXTURE_2D_ARRAY, GL_DEPTH_COMPONENT24, map_width, map_height, num_layers, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, true)),
        fbo(map_width, map_height)
    {
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
        program->SetMat4("lightSpaceMatrix", dl.GetLightspaceMatrix());
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
            program->SetMat4("lightSpaceMatrices[" + std::to_string(j) + "]", pl.GetLightspaceMatrices()[j]);
        }
        program->SetVec3("lightPos", position);
        program->SetFloat("far", pl.GetFarPlane());
    }

    void PointShadowModule::SetObjectUniforms(const glm::mat4& model) {
        program->SetMat4("model", model);
    }

    std::shared_ptr<Core::Program> PointShadowModule::GetProgram() {
        return program;
    }

    LightingPassPBRModule::LightingPassPBRModule() {}

    void LightingPassPBRModule::SetGlobalUniforms() {
        program->SetInt("gPosition", gPosition);
        program->SetInt("gNormal", gNormal);
        program->SetInt("gAlbedoSpec", gAlbedoSpec);
        program->SetInt("gMetRouOcc", gMetRouOcc);

        program->SetInt("irradianceMap", irradianceMap);
        program->SetInt("prefilterMap", prefilterMap);
        program->SetInt("brdfLUT", brdfLUT);
        program->SetFloat("ibl", ibl);

        program->SetInt("shadowmap_2d_array_shadow", shadowmap_2d_array_shadow);
        program->SetInt("shadowmap_cube_array_shadow", shadowmap_cube_array_shadow);
    }

    std::shared_ptr<Core::Program> LightingPassPBRModule::GetProgram() {
        return program;
    }

    SkyboxModule::SkyboxModule() {}
    
    void SkyboxModule::SetGlobalUniforms(Component::Camera& camera, Core::Tex2D& skybox, int tex_unit) {
        program->SetMat4("view", glm::mat4(glm::mat3(camera.View())));
        program->SetMat4("projection", camera.projection);
        program->SetInt("cubemap", tex_unit);
    }

    std::shared_ptr<Core::Program> SkyboxModule::GetProgram() {
        return program;
    }

}