#include <plum/scene/environment.hpp>

#include <plum/asset/manager.hpp>
#include <plum/component/primitive.hpp>
#include <plum/core/globject.hpp>
#include <plum/interface/widget.hpp>

#include <glm/glm.hpp>


#include <iostream>
#include <vector>

namespace Scene {

    Environment::Environment() 
        : Environment(nullptr)
    {}

    Environment::Environment(std::shared_ptr<Material::Texture> envmap)
        : envmap(envmap)
    {
        if (!equirectProgram) {
            AssetManager& manager = AssetManager::Instance();
            
            auto vsEquirect = manager.LoadHot<ShaderAsset>("shaders/shaderv_equirect.vs", GL_VERTEX_SHADER);
            auto fsEquirect = manager.LoadHot<ShaderAsset>("shaders/shaderf_equirect.fs", GL_FRAGMENT_SHADER);
            auto fsIrradiance = manager.LoadHot<ShaderAsset>("shaders/shaderf_irradiance.fs", GL_FRAGMENT_SHADER);
            auto fsPrefilter = manager.LoadHot<ShaderAsset>("shaders/shaderf_prefilter.fs", GL_FRAGMENT_SHADER);
            
            equirectProgram = std::make_shared<Core::Program>(vsEquirect, fsEquirect);
            irradianceProgram = std::make_shared<Core::Program>(vsEquirect, fsIrradiance);
            prefilterProgram = std::make_shared<Core::Program>(vsEquirect, fsPrefilter);
            
            auto vs2d = manager.LoadHot<ShaderAsset>("shaders/shaderv_2d.vs", GL_VERTEX_SHADER);
            auto fsBrdfLut = manager.LoadHot<ShaderAsset>("shaders/shaderf_2dbrdflut.fs", GL_FRAGMENT_SHADER);
            
            brdfLutProgram = std::make_shared<Core::Program>(vs2d, fsBrdfLut);
        }
        if (envmap) {
            if (envmap->tex->target == GL_TEXTURE_CUBE_MAP) {
                skybox = envmap->tex;
            } else if (envmap->tex->target == GL_TEXTURE_2D) {
                skybox = equirectToCubemap(envmap->tex, 2048, 2048);
            } else {
                throw std::runtime_error("Environment map texture must be of type GL_TEXTURE_CUBE_MAP or GL_TEXTURE_2D!");
            }
            cubemapToIrradiance(64, 64);
            cubemapToPrefilter(256, 256, skybox->width);
            generateBrdfLut(512, 512);
        }
    }

    void Environment::DrawSkybox(Component::Camera& camera) {
        if (skybox)
            skyboxModule.Render(*skybox, camera);
    }

    void Environment::DisplayWidget() {
        static bool showChild = false;
        static bool firstDisplay = true;
        static int sel = 0;
        if (firstDisplay && envmap) {
            if (envmap->tex->target == GL_TEXTURE_2D) {
                sel = 1;
            } else if (envmap->tex->target == GL_TEXTURE_CUBE_MAP) {
                sel = 2;
            } else {
                sel = 0;
            }
        }
        ImGui::RadioButton("None", &sel, 0); ImGui::SameLine();
        ImGui::RadioButton("Equirectangular", &sel, 1); ImGui::SameLine();
        ImGui::RadioButton("Six Sided", &sel, 2);
        static const Directory skyboxesDir("assets/skyboxes");
        switch (sel) {
            case 0:
                if (ImGui::Button("Save")) {
                    if (skybox) {
                        *this = Environment();
                    }
                    showChild = false;
                }
                break;
            case 1:
            {
                static Interface::PathComboWidget pathComboWidget;
                static Path skyboxPath = Path();
                static bool flip = true;
                if (firstDisplay) {
                    skyboxPath = envmap->images[0]->GetFile();
                    flip = envmap->images[0]->Flip();
                    firstDisplay = false;
                }
                pathComboWidget.Display(skyboxesDir, "Path", AssetUtils::imageExtensions, &skyboxPath, Path());
                ImGui::SameLine(); 
                ImGui::Checkbox("Flip", &flip);
                if (ImGui::Button("Save")) {
                    if (!skyboxPath.IsEmpty()) {
                        auto image = AssetManager::Instance().LoadHot<ImageAsset>(skyboxPath, flip);
                        auto texture = std::make_shared<Material::Texture>(image, Material::TextureType::Diffuse, GL_CLAMP_TO_EDGE, GL_LINEAR);
                        *this = Environment(texture);
                        showChild = false;
                    }
                }
                break;
            }
            case 2:
            {
                static Interface::PathComboWidget pathComboWidgets[6];
                constexpr const char* faces[] = {"+X", "-X", "+Y", "-Y", "+Z", "-Z"};
                static std::vector<Path> facePaths(6);
                static bool flips[6] = {true, true, true, true, true, true};
                if (firstDisplay) {
                    for (int i = 0; i < 6; i++) {
                        facePaths[i] = envmap->images[i]->GetFile();
                        flips[i] = envmap->images[i]->Flip();
                    }
                    firstDisplay = false;
                }
                for (int i = 0; i < 6; i++) {
                    pathComboWidgets[0].Display(skyboxesDir, faces[i], AssetUtils::imageExtensions, &facePaths[i], Path());
                    ImGui::SameLine();
                    const std::string strId = std::string("Flip") + std::to_string(i);
                    ImGui::Checkbox(strId.c_str(), &flips[i]);
                }
                if (ImGui::Button("Save")) {
                    bool pathsValid = true;
                    for (const Path& path : facePaths) {
                        if (path.IsEmpty()) {
                            pathsValid = false;
                            break;
                        }
                    }
                    if (pathsValid) {
                        std::vector<std::shared_ptr<ImageAsset>> images;
                        for (int i = 0; i < facePaths.size(); i++) {
                            images.emplace_back(AssetManager::Instance().LoadHot<ImageAsset>(facePaths[i], flips[i]));
                        }
                        auto texture = std::make_shared<Material::Texture>(images, Material::TextureType::Diffuse, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR);
                        *this = Environment(texture);
                        showChild = false;
                    }
                }
                break;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            showChild = false;
        }
    }

    std::shared_ptr<Core::Tex2D> Environment::equirectToCubemap(std::shared_ptr<Core::Tex2D> equirect, int width, int height) {
        
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

        cubemap->Bind();
        cubemap->GenerateMipMap();

        return cubemap;
    }

    void Environment::cubemapToIrradiance(int width, int height) {
       
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

    void Environment::cubemapToPrefilter(int width, int height, int envres) {

        Core::Fbo fbo(width, height);
        prefilter = std::make_shared<Core::Tex2D>(GL_TEXTURE_CUBE_MAP, GL_RGB32F, fbo.width, fbo.height, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, false, true);
        prefilter->Bind();
        prefilter->GenerateMipMap();    // Allocate memory for the upcoming mipmap levels to be rendered to
        
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
            
            int mipWidth = static_cast<int>(fbo.width * std::pow(0.5, mip));
            int mipHeight = static_cast<int>(fbo.height * std::pow(0.5, mip));
            
            fbo.depthRboAtt->Bind();
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);
            float roughness = static_cast<float>(mip) / static_cast<float>(maxMipLevels - 1);
            prefilterProgram->SetFloat("roughness", roughness);
            
            fbo.CheckStatus();
            for (int i = 0; i < 6; i++) {
                fbo.AttachColorTexCubeFace(0, i, mip);  // Can't call Bind() because it clears the depth rbo?
                fbo.ClearColor();
                fbo.ClearDepth();
                prefilterProgram->SetMat4("view", views[i]);
                glCullFace(GL_FRONT);
                cube.vao->Draw();
                glCullFace(GL_BACK);
            }
        }

        // prefilter->GenerateMipMap();    // Actually generate mipmaps of the image
    }

    void Environment::generateBrdfLut(int width, int height) {

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