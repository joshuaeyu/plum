#include "demo1.hpp"

#include <plum/component/all.hpp>
#include <plum/asset/image.hpp>
#include <plum/asset/manager.hpp>
#include <plum/interface/widget.hpp>
#include <plum/util/time.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include <iostream>
#include <vector>
#include <string>

Demo1::Demo1()
    : Demo("Demo1")
{}

void Demo1::Initialize() {
    std::clog << "Creating renderer..." << std::endl;
    renderer = std::make_unique<Renderer::DeferredRenderer>();
    fxaa = std::make_unique<PostProcessing::Fxaa>();
    hdr = std::make_unique<PostProcessing::Hdr>();
    bloom = std::make_unique<PostProcessing::Bloom>();

    std::clog << "Setting up environment..." << std::endl;
    auto kloppenheim = AssetManager::Instance().LoadHot<ImageAsset>("assets/skyboxes/kloppenheim_4k.hdr");
    auto skybox = std::make_shared<Material::Texture>(kloppenheim, Material::TextureType::Diffuse);
    environment = std::make_unique<Scene::Environment>(skybox);
    
    std::clog << "Defining materials..." << std::endl;
    auto copper = std::make_shared<Material::PBRMetallicMaterial>();
    copper->name = "Copper";
    copper->albedo = glm::pow(glm::vec3(250, 208, 192)/255.f, glm::vec3(2.2));
    copper->metallic = 1.0;
    copper->roughness = 0.3;
    materials.insert(copper);
    auto iron = std::make_shared<Material::PBRMetallicMaterial>();
    iron->name = "Iron";
    iron->albedo = glm::pow(glm::vec3(198, 198, 200)/255.f, glm::vec3(2.2));
    iron->metallic = 1.0;
    iron->roughness = 0.3;
    materials.insert(iron);
    auto gold = std::make_shared<Material::PBRMetallicMaterial>();
    gold->name = "Gold";
    gold->albedo = glm::pow(glm::vec3(255, 226, 155)/255.f, glm::vec3(2.2));
    gold->metallic = 1.0;
    gold->roughness = 0.3;
    materials.insert(gold);

    std::clog << "Creating components..." << std::endl;
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
    sphere->material = gold;
    auto cube = std::make_shared<Component::Cube>();
    cube->material = iron;

    std::clog << "Loading models..." << std::endl;
    // auto backpack = std::make_shared<Component::Model>("assets/models/survival_guitar_backpack/scene.gltf", 0.005f);
    // models["Backpack"] = backpack;
    // auto sponzaAsset = AssetManager::Instance().LoadHot<ModelAsset>("assets/models/sponza/glTF/Sponza.gltf");
    // auto sponza = std::make_shared<Component::Model>(sponzaAsset);

    std::clog << "Defining scene..." << std::endl;
    scene = std::make_unique<Scene::Scene>();
    auto dlNode = scene->EmplaceChild(dirlight);
    dlNode->transform.Rotate(80,0,0);
    auto plNode = scene->EmplaceChild(pointlight);
    plNode->transform.Translate(0,5,0);
    auto planeNode = scene->EmplaceChild(plane);
    planeNode->transform.Scale(25);
    planeNode->transform.Translate(0,-5,0);
    auto sphereNode = scene->EmplaceChild(sphere);
    sphereNode->transform.Translate(0,2,0);
    auto cubeNode = sphereNode->EmplaceChild(cube);
    cubeNode->transform.Translate(0,2,0);
    // auto backpackNode = scene->EmplaceChild(backpack);
    // backpackNode->transform.Translate(5,4,0);
    // auto sponzaNode = scene->EmplaceChild(sponza);
    // sponzaNode->name = "Sponza";

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

    while (GLenum error = glGetError()) { std::cerr << "Render error: " << error << std::endl; }
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
    
    constexpr float pointsPerSecond = 1.f;    // points/sec
    constexpr int secondsToDisplay = 20.f; // sec / points/sec
    static Interface::PerformanceWidget performanceWidget;
    performanceWidget.Display(pointsPerSecond, secondsToDisplay);

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
        static Directory displayDir("assets");
        static const Directory highestDir("assets");
        static Interface::FileExplorerWidget fileExplorerWidget;
        bool changed = fileExplorerWidget.Display(&displayDir, highestDir);
    }

    if (ImGui::CollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen)) {
        static bool showMaterialCreationWidget = false;
        if (ImGui::Button("New Material")) {
            showMaterialCreationWidget = !showMaterialCreationWidget;
        }
        if (showMaterialCreationWidget) {
            static Interface::MaterialCreationWidget materialCreationWidget;
            std::shared_ptr<Material::MaterialBase> mat;
            if (materialCreationWidget.Display(&mat)) {
                std::string newName = mat->name;
                bool nameExists = true;
                int i = 1;
                while (nameExists) {
                    nameExists = false;
                    for (auto& material : materials) {
                        if (newName == material->name) {
                            nameExists = true;
                            newName = mat->name + "-" + std::to_string(i++);
                            break;
                        }
                    }
                }
                mat->name = newName;
                materials.insert(mat);
                showMaterialCreationWidget = false;
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
            environment->DisplayWidget();
            ImGui::TreePop();
        }
    }

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
            static Interface::ComponentCreationWidget componentCreationWidget;
            std::shared_ptr<Component::ComponentBase> comp;
            if (componentCreationWidget.Display(&comp)) {
                scene->EmplaceChild(comp);
                showComponentCreationWidget = false;
            }
        }
        scene->DisplayWidget(materials);
    }
    ImGui::End();
}