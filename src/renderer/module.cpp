#include "renderer/module.hpp"

#include "component/camera.hpp"
#include "component/light.hpp"
#include "component/primitive.hpp"
#include "asset/manager.hpp"
#include "scene/scene.hpp"
#include "scene/scenenode.hpp"

#include <set>

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
            AssetManager& manager = AssetManager::Instance();
            
            auto vs = manager.LoadHot<ShaderAsset>("assets/shaders/shaderv_shadow2d.vs", GL_VERTEX_SHADER);
            auto fs = manager.LoadHot<ShaderAsset>("assets/shaders/shaderf_shadow2d.fs", GL_FRAGMENT_SHADER);
            
            program = std::make_shared<Core::Program>(vs, fs);
            program->SetUniformBlockBindingScheme(Core::Program::UboScheme::Scheme1);
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
        
        for (int i = 0, shadow_count = 0; i < dirlight_nodes.size() && shadow_count < numLayers; i++) {
            Component::DirectionalLight& dirlight = dynamic_cast<Component::DirectionalLight&>(*dirlight_nodes[i]->component);
            
            if (dirlight.HasShadows()) {
                setGlobalUniforms(dirlight, fbo.depthAtt->Handle(), &shadow_count);
                
                glCullFace(GL_FRONT);
                scene.Draw(*this);
                glCullFace(GL_BACK);
            }
        }
    }
    
    void DirectionalShadowModule::SetObjectUniforms(const glm::mat4& model, const Component::ComponentBase& component) {
        program->SetMat4("model", model);
    }

    bool DirectionalShadowModule::AllowDraw(const Component::ComponentBase& component) {
        if (component.IsMesh()) {
            return dynamic_cast<const Component::Mesh&>(component).castShadows;
        }
        return true;
    }

    void DirectionalShadowModule::setGlobalUniforms(Component::DirectionalLight& dl, GLuint depth_texture, int* shadow_idx) {
        program->SetMat4("lightSpaceMatrix", dl.LightspaceMatrix());
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0, (*shadow_idx)++);
    }
    
    PointShadowModule::PointShadowModule(int map_width, int map_height, int num_layers)
        : mapWidth(map_width),
        mapHeight(map_height),
        numLayers(num_layers),
        depthMap(std::make_shared<Core::Tex3D>(GL_TEXTURE_CUBE_MAP_ARRAY, GL_DEPTH_COMPONENT24, map_width, map_height, 6 * num_layers, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, true)),
        fbo(map_width, map_height)
    {
        if (!program) {
            AssetManager& manager = AssetManager::Instance();
            auto vs = manager.LoadHot<ShaderAsset>("assets/shaders/shaderv_shadowcube.vs", GL_VERTEX_SHADER);
            auto fs = manager.LoadHot<ShaderAsset>("assets/shaders/shaderf_shadowcube.fs", GL_FRAGMENT_SHADER);
            auto gs = manager.LoadHot<ShaderAsset>("assets/shaders/shaderg_shadowcube.gs", GL_GEOMETRY_SHADER);
            
            program = std::make_shared<Core::Program>(vs, fs, gs);
            program->SetUniformBlockBindingScheme(Core::Program::UboScheme::Scheme1);
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

        for (int i = 0, shadow_count = 0; i < pointlight_nodes.size() && shadow_count < numLayers; i++) {
            Component::PointLight& pointlight = dynamic_cast<Component::PointLight&>(*pointlight_nodes[i]->component);

            if (pointlight.HasShadows()) {
                glm::mat4 worldTransform = glm::identity<glm::mat4>();
                Scene::SceneNode* currNode = pointlight_nodes[i];
                while (currNode) {
                    worldTransform = currNode->transform.Matrix() * worldTransform;
                    currNode = currNode->parent;
                }
                setGlobalUniforms(pointlight, worldTransform[3], &shadow_count);
                
                glCullFace(GL_FRONT);
                scene.Draw(*this);
                glCullFace(GL_BACK);
            }
        }
    }

    void PointShadowModule::SetObjectUniforms(const glm::mat4& model, const Component::ComponentBase& component) {
        program->SetMat4("model", model);
    }

    bool PointShadowModule::AllowDraw(const Component::ComponentBase& component) {
        if (component.IsMesh()) {
            return static_cast<const Component::Mesh&>(component).castShadows;
        }
        return true;
    }

    void PointShadowModule::setGlobalUniforms(Component::PointLight& pl, const glm::vec3& position, int* shadow_idx) {
        program->SetInt("layer", (*shadow_idx)++);
        for (int j = 0; j < 6; j++) {
            program->SetMat4("lightSpaceMatrices[" + std::to_string(j) + "]", pl.LightspaceMatrices()[j]);
        }
        program->SetVec3("lightPos", position);
        program->SetFloat("far", pl.FarPlane());
    }

    SkyboxModule::SkyboxModule() {
        if (!program) {
            AssetManager& manager = AssetManager::Instance();
            auto vs = manager.LoadHot<ShaderAsset>("assets/shaders/shaderv_skybox.vs", GL_VERTEX_SHADER);
            auto fs = manager.LoadHot<ShaderAsset>("assets/shaders/shaderf_skybox.fs", GL_FRAGMENT_SHADER);
            
            program = std::make_shared<Core::Program>(vs, fs);
            program->SetUniformBlockBindingScheme(Core::Program::UboScheme::Scheme1);
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

    SsaoModule::SsaoModule()
        : ssao(std::make_shared<Core::Tex2D>(GL_TEXTURE_2D, GL_RED, 2, 2, GL_RED, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR)),
        ssaoBlurred(std::make_shared<Core::Tex2D>(GL_TEXTURE_2D, GL_RED, 2, 2, GL_RED, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR)),
        noise(GL_TEXTURE_2D, GL_RGBA16F, 4, 4, GL_RGB, GL_FLOAT, GL_REPEAT, GL_NEAREST),
        fbo(2,2)
    {
        if (!program) {
            AssetManager& manager = AssetManager::Instance();

            auto vs2d = manager.LoadHot<ShaderAsset>("assets/shaders/shaderv_2d.vs", GL_VERTEX_SHADER);
            auto fsSsao = manager.LoadHot<ShaderAsset>("assets/shaders/shaderf_2dssao.fs", GL_FRAGMENT_SHADER);
            auto fsSsaoBlur = manager.LoadHot<ShaderAsset>("assets/shaders/shaderf_2dssaoBlur.fs", GL_FRAGMENT_SHADER);
            
            program = std::make_shared<Core::Program>(vs2d, fsSsao);
            program->SetUniformBlockBindingScheme(Core::Program::UboScheme::Scheme1);

            programBlur = std::make_shared<Core::Program>(vs2d, fsSsaoBlur);
            programBlur->SetUniformBlockBindingScheme(Core::Program::UboScheme::Scheme1);
        }
        fbo.Bind();
        fbo.AttachColorTex(ssao);
        fbo.AttachColorTex(ssaoBlurred);
        fbo.CheckStatus();

        // Generate sampling kernel (random vectors in tangent space in the +normal hemisphere)
        std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
        std::default_random_engine generator;
        for (int i = 0; i < 64; i++) {
            glm::vec3 sample(
                randomFloats(generator) * 2.0f - 1.0f,
                randomFloats(generator) * 2.0f - 1.0f,
                randomFloats(generator)
            );
            sample = randomFloats(generator) * glm::normalize(sample);
            float scale = static_cast<float>(i) / 64.0f;
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

}