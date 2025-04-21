#include "demo1.hpp"

#include <plum/component/all.hpp>
#include <plum/asset/image.hpp>
#include <plum/asset/manager.hpp>
#include <plum/interface/widget.hpp>
#include <plum/util/time.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

#include <iostream>
#include <vector>
#include <string>

Demo1::Demo1()
    : Demo("Demo1")
{}

void Demo1::Initialize() {
    std::cout << "Setting up environment..." << std::endl;
    auto kloppenheim = AssetManager::Instance().LoadHot<ImageAsset>("assets/skyboxes/kloppenheim_4k.hdr");
    auto skybox = std::make_shared<Material::Texture>(kloppenheim, Material::TextureType::Diffuse);
    environment = std::make_unique<Scene::Environment>(skybox->tex);
    
    std::cout << "Defining materials..." << std::endl;
    auto copper = std::make_shared<Material::PBRMetallicMaterial>();
    copper->name = "Copper";
    copper->albedo = glm::pow(glm::vec3(0.72,0.45,0.22),glm::vec3(2.2));
    copper->metallic = 1.0;
    copper->roughness = 0.15;
    materials.insert(copper);
    auto ruby = std::make_shared<Material::PBRMetallicMaterial>();
    ruby->name = "Ruby";
    ruby->albedo = glm::pow(glm::vec3(0.6,0.1,0.1),glm::vec3(2.2));
    ruby->metallic = 0.0;
    ruby->roughness = 0.1;
    materials.insert(ruby);
    auto sapphire = std::make_shared<Material::PBRMetallicMaterial>();
    sapphire->name = "Sapphire";
    sapphire->albedo = glm::pow(glm::vec3(0.1,0.2,0.7),glm::vec3(2.2));
    sapphire->metallic = 1.0;
    sapphire->roughness = 0.2;
    materials.insert(sapphire);

    std::cout << "Creating components..." << std::endl;
    camera = std::make_unique<Component::Camera>();
    camera->Translate(-5,3,-0.25);
    camera->Rotate(0,-90);
    auto dirlight = std::make_shared<Component::DirectionalLight>();
    dirlight->color = glm::vec3(0.5,0.5,1.0);
    dirlight->intensity = 10.f;
    dirlight->EnableShadows();
    auto pointlight = std::make_shared<Component::PointLight>();
    pointlight->color = glm::vec3(1.0,1.0,0.5);
    pointlight->intensity = 10.f;
    pointlight->EnableShadows();
    auto plane = std::make_shared<Component::Plane>();
    plane->material = copper;
    auto sphere = std::make_shared<Component::Sphere>();
    sphere->material = sapphire;
    auto cube = std::make_shared<Component::Cube>();
    cube->material = ruby;

    std::cout << "Loading models..." << std::endl;
    // auto backpack = std::make_shared<Component::Model>("assets/models/survival_guitar_backpack/scene.gltf", 0.005f);
    // models["Backpack"] = backpack;
    // auto sponza = std::make_shared<Component::Model>("assets/models/sponza/glTF/Sponza.gltf");
    // models["Sponza"] = sponza;

    std::cout << "Defining scene..." << std::endl;
    scene = std::make_unique<Scene::Scene>();
    auto dlNode = scene->EmplaceChild(dirlight);
    dlNode->transform.Rotate(80,0,0);
    auto plNode = scene->EmplaceChild(pointlight);
    plNode->transform.Translate(0,5,0);
    auto planeNode = scene->EmplaceChild(plane);
    planeNode->transform.Scale(25);
    planeNode->transform.Translate(0,-5,0);
    auto cubeNode = scene->EmplaceChild(cube);
    cubeNode->transform.Translate(0,2,0);
    auto sphereNode = cubeNode->EmplaceChild(sphere);
    sphereNode->transform.Translate(0,2,0);
    // auto backpackNode = scene->EmplaceChild(backpack);
    // backpackNode->transform.Translate(5,4,0);
    // auto sponzaNode = scene->EmplaceChild(sponza);
    
    std::cout << "Creating renderer..." << std::endl;
    renderer = std::make_unique<Renderer::DeferredRenderer>();
    fxaa = std::make_unique<PostProcessing::Fxaa>();
    hdr = std::make_unique<PostProcessing::Hdr>();
    bloom = std::make_unique<PostProcessing::Bloom>();
    while (GLenum error = glGetError()) { std::cerr << "Initialization error: " << error << std::endl; }
}
void Demo1::Display() {
    camera->ProcessInputs();
    
    // backpackNode->transform.Rotate(glm::vec3(0,30,0) * app.DeltaTime());
    // cubeNode->transform.Rotate(glm::vec3(50,120,90) * app.DeltaTime());

    environment->iblIntensity = renderOptions.iblIntensity;
    renderer->ssao = renderOptions.ssao;

    static Core::Fbo* fbo;
    fbo = renderer->Render(*scene, *camera, *environment);
    if (renderOptions.bloom) {
        fbo = bloom->Process(*fbo);
    }
    if (renderOptions.hdr) {
        hdr->exposure = renderOptions.hdrExposure;
        fbo = hdr->Process(*fbo);
    }
    if (renderOptions.fxaa) {
        fbo = fxaa->Process(*fbo);
    }
    fbo->BlitToDefault();

    displayGui();
}
void Demo1::CleanUp() {
    scene.reset();
    environment.reset();
    camera.reset();
    renderer.reset();
    fxaa.reset();
    hdr.reset();
    bloom.reset();
}

void Demo1::displayGui() {
    if (!ImGui::Begin("Plum Engine v2.00 Beta", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings)) {
        ImGui::End();
        return;
    }
    ImGui::SetWindowPos(ImVec2(0,0));
    ImGui::TextColored(ImVec4(0.3,1,1,1), "Press ` to capture/release mouse.");
    ImGui::TextColored(ImVec4(0.3,1,1,1), "Use WASD, Shift, and Spacebar to move camera.");
    ImGui::Spacing();
    
    constexpr float pointsPerSecond = 0.5f;    // points/sec
    constexpr int secondsToDisplay = 20.f; // sec / points/sec
    Widget::PerformanceWidget(pointsPerSecond, secondsToDisplay);

    if (ImGui::CollapsingHeader("Render Options", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("IBL Intensity", &renderOptions.iblIntensity, 0.0f, 1.0f);
        ImGui::Checkbox("SSAO", &renderOptions.ssao); ImGui::SameLine();
        ImGui::Checkbox("FXAA", &renderOptions.fxaa); ImGui::SameLine();
        ImGui::Checkbox("Bloom", &renderOptions.bloom);
        ImGui::Checkbox("HDR", &renderOptions.hdr);
        if (renderOptions.hdr) {
            ImGui::SameLine();
            ImGui::SliderFloat("Exposure", &renderOptions.hdrExposure, 0.0f, 10.0f);
        }
    }
    
    if (ImGui::CollapsingHeader("File Explorer", ImGuiTreeNodeFlags_DefaultOpen)) {
        static Path displayPath("assets");
        static const Path highestPath("assets");
        displayPath = Widget::FileExplorerWidget(displayPath, highestPath);
    }

    if (ImGui::CollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Engine object needs to mirror file itself
        // list all files at component dialog, or just models that have already been imported?
        static bool showMaterialCreationWidget = false;
        if (ImGui::Button("New Material")) {
            showMaterialCreationWidget = !showMaterialCreationWidget;
        }
        if (showMaterialCreationWidget) {
            auto mat = Widget::MaterialCreationWidget(&showMaterialCreationWidget);
            if (mat) {
                std::string newName = mat->name;
                bool nameExists = true;
                int i = 1;
                while (nameExists) {
                    nameExists = false;
                    for (auto& material : materials) {
                        if (mat->name == material->name) {
                            nameExists = true;
                            newName = mat->name + "-" + std::to_string(i++);
                            break;
                        }
                    }
                }
                mat->name = newName;
                materials.insert(mat);
            }
        }
        for (auto& material : materials) {
            if (ImGui::TreeNode(material->name.c_str())) {
                material->DisplayWidget();
                ImGui::TreePop();
            }
        }
    }
    if (ImGui::CollapsingHeader("Environment", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::TreeNode("Skybox")) {
            static bool showChild = false;
            if (!showChild && ImGui::Button("Edit")) {
                showChild = !showChild;
            }
            if (showChild) {
                static int sel = 0;
                ImGui::RadioButton("None", &sel, 0); ImGui::SameLine();
                ImGui::RadioButton("Equirectangular", &sel, 1); ImGui::SameLine();
                ImGui::RadioButton("Six Sided", &sel, 2);
                static const Directory skyboxesDir("assets/skyboxes");
                switch (sel) {
                    case 0:
                        if (ImGui::Button("Save")) {
                            if (environment->skybox) {
                                *environment = Scene::Environment();
                            }
                            showChild = !showChild;
                        }
                        break;
                    case 1:
                    {
                        static int widgetId = -1;
                        static Path skyboxPath = Path();
                        static bool flip = true;
                        Widget::PathComboWidget(&widgetId, skyboxesDir, "Path", AssetUtils::imageExtensions, &skyboxPath, Path());
                        ImGui::SameLine(); ImGui::Checkbox("Flip", &flip);
                        if (ImGui::Button("Save")) {
                            if (!skyboxPath.IsEmpty()) {
                                auto image = AssetManager::Instance().LoadHot<ImageAsset>(skyboxPath, flip);
                                auto texture = std::make_shared<Material::Texture>(image, Material::TextureType::Diffuse, GL_CLAMP_TO_EDGE, GL_LINEAR);
                                *environment = Scene::Environment(texture->tex);
                                showChild = !showChild;
                            }
                        }
                    }
                        break;
                    case 2:
                    {
                        static int widgetIds[6] = {-1, -1, -1, -1, -1, -1};
                        constexpr const char* faces[] = {"+X", "-X", "+Y", "-Y", "+Z", "-Z"};
                        static std::vector<Path> facePaths(6);
                        static bool flips[6] = {true, true, true, true, true, true};
                        Widget::PathComboWidget(&widgetIds[0], skyboxesDir, "+X", AssetUtils::imageExtensions, &facePaths[0], Path());
                        ImGui::SameLine(); ImGui::Checkbox("Flip##0", &flips[0]);
                        Widget::PathComboWidget(&widgetIds[1], skyboxesDir, "-X", AssetUtils::imageExtensions, &facePaths[1], Path());
                        ImGui::SameLine(); ImGui::Checkbox("Flip##1", &flips[1]);
                        Widget::PathComboWidget(&widgetIds[2], skyboxesDir, "+Y", AssetUtils::imageExtensions, &facePaths[2], Path());
                        ImGui::SameLine(); ImGui::Checkbox("Flip##2", &flips[2]);
                        Widget::PathComboWidget(&widgetIds[3], skyboxesDir, "-Y", AssetUtils::imageExtensions, &facePaths[3], Path());
                        ImGui::SameLine(); ImGui::Checkbox("Flip##3", &flips[3]);
                        Widget::PathComboWidget(&widgetIds[4], skyboxesDir, "+Z", AssetUtils::imageExtensions, &facePaths[4], Path());
                        ImGui::SameLine(); ImGui::Checkbox("Flip##4", &flips[4]);
                        Widget::PathComboWidget(&widgetIds[5], skyboxesDir, "-Z", AssetUtils::imageExtensions, &facePaths[5], Path());
                        ImGui::SameLine(); ImGui::Checkbox("Flip##5", &flips[5]);
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
                                auto texture = std::make_shared<Material::Texture>(images, Material::TextureType::Diffuse, GL_CLAMP_TO_EDGE, GL_LINEAR);
                                *environment = Scene::Environment(texture->tex);
                                showChild = !showChild;
                            }
                        }
                    }
                        break;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                showChild = !showChild;
            }
            ImGui::TreePop();
        }
    }

    int i = 0;
    if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("New Empty Node")) {
            scene->EmplaceChild();
        }
        ImGui::SameLine();
        static bool showComponentCreationWidget = false;
        if (ImGui::Button("New Component Node")) {
            showComponentCreationWidget = !showComponentCreationWidget;
        }
        if (showComponentCreationWidget) {
            auto component = Widget::ComponentCreationWidget(&showComponentCreationWidget);
            if (component) {
                scene->EmplaceChild(component);
            }
        }
        for (auto& child : scene->children) {
            bool deleteRequested = !gui_DisplaySceneNode(*child, i);
            if (deleteRequested) {
                scene->RemoveChild(child);
                break;  // Don't iterate further
            }
        }
    }
    ImGui::End();
}

bool Demo1::gui_DisplaySceneNode(Scene::SceneNode& node, int& i) {
    const std::string nodeName = node.name+"##"+std::to_string(i++);
    bool expanded = ImGui::TreeNodeEx(nodeName.c_str());
    // Right-click context menu
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::Button("Duplicate")) {
            // future
        }
        if (ImGui::Button("Delete")) {
            ImGui::EndPopup();
            if (expanded)
                ImGui::TreePop();
            return false;
        }
        ImGui::EndPopup();
    }
    // Node Transform and children
    if (expanded) {
        if (ImGui::Button("Delete Node")) {
            ImGui::TreePop();
            return false;
        }
        if (ImGui::TreeNodeEx("[Transform]")) {
            bool pos = ImGui::DragFloat3("Position", glm::value_ptr(node.transform.position), 0.01f, 0.0f, 0.0f, "%.2f");
            bool rot = ImGui::DragFloat3("Rotation", glm::value_ptr(node.transform.rotationEuler), 0.1f, 0.0f, 0.0f, "%.1f");
            bool scale = ImGui::DragFloat3("Scale", glm::value_ptr(node.transform.scale), 0.001f, 0.001f, 1e6f, "%.3f");
            if (pos || rot || scale)
                node.transform.Update();
            ImGui::TreePop();
        }
        if (!node.component) {
            static bool showComponentCreationWidget = false;
            if (ImGui::Button("Add Component")) {
                showComponentCreationWidget = !showComponentCreationWidget;
            }
            if (showComponentCreationWidget) {
                auto component = Widget::ComponentCreationWidget(&showComponentCreationWidget);
                if (component) {
                    node.component = component;
                }
            }
        } else {
            if (ImGui::TreeNodeEx(("[" + node.component->name + "]").c_str())) {
                if (node.component->IsMesh()) {
                    std::static_pointer_cast<Component::Mesh>(node.component)->DisplayWidget(materials);
                } else {
                    node.component->DisplayWidget();
                }
                if (ImGui::Button("Remove Component")) {
                    node.component.reset();
                }
                ImGui::TreePop();
            }
        }
        for (auto& child : node.children) {
            if (!gui_DisplaySceneNode(*child, i))
                node.RemoveChild(child);
        }
        ImGui::TreePop();
    }
    return true;
}