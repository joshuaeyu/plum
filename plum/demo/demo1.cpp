#include "demo1.hpp"

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
    // auto skybox = std::make_shared<Material::Texture>("assets/textures/black.png", Material::TextureType::Diffuse);
    // auto skybox = std::make_shared<Material::Texture>("assets/textures/dresden_station_4k.hdr", Material::TextureType::Diffuse);
    auto skybox = std::make_shared<Material::Texture>("assets/textures/kloppenheim_4k.hdr", Material::TextureType::Diffuse);
    // static std::vector<std::string> oceanSkyboxPaths = {
    //     "assets/textures/skybox/right.jpg",
    //     "assets/textures/skybox/left.jpg",
    //     "assets/textures/skybox/top.jpg",
    //     "assets/textures/skybox/bottom.jpg",
    //     "assets/textures/skybox/front.jpg",
    //     "assets/textures/skybox/back.jpg"
    // };
    // static auto skybox = std::make_shared<Material::Texture>(oceanSkyboxPaths, Material::TextureType::Diffuse, false);
    environment = std::make_unique<Scene::Environment>(skybox->tex);
    
    std::cout << "Defining materials..." << std::endl;
    auto copper = std::make_shared<Material::PBRMetallicMaterial>();
    copper->albedo = glm::pow(glm::vec3(0.72,0.45,0.22),glm::vec3(2.2));
    copper->metallic = 1.0;
    copper->roughness = 0.15;
    auto ruby = std::make_shared<Material::PBRMetallicMaterial>();
    ruby->albedo = glm::pow(glm::vec3(0.6,0.1,0.1),glm::vec3(2.2));
    ruby->metallic = 0.0;
    ruby->roughness = 0.1;
    auto sapphire = std::make_shared<Material::PBRMetallicMaterial>();
    sapphire->albedo = glm::pow(glm::vec3(0.1,0.2,0.7),glm::vec3(2.2));
    sapphire->metallic = 1.0;
    sapphire->roughness = 0.2;

    std::cout << "Creating components..." << std::endl;
    camera = std::make_unique<Component::Camera>();
    camera->transform.Translate(-5,3,-0.25);
    camera->Rotate(-90,0);
    camera->transform.Update();
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
    auto backpack = std::make_shared<Component::Model>("assets/models/survival_guitar_backpack/scene.gltf", 0.005f);
    auto sponza = std::make_shared<Component::Model>("assets/models/sponza/glTF/Sponza.gltf");

    std::cout << "Defining scene..." << std::endl;
    scene = std::make_unique<Scene::Scene>();
    auto dlNode = scene->AddChild(dirlight);
    dlNode->transform.Rotate(80,0,0);
    auto plNode = scene->AddChild(pointlight);
    plNode->transform.Translate(0,5,0);
    auto planeNode = scene->AddChild(plane);
    planeNode->transform.Scale(25);
    planeNode->transform.Translate(0,-5,0);
    auto cubeNode = scene->AddChild(cube);
    cubeNode->transform.Translate(0,2,0);
    auto sphereNode = cubeNode->AddChild(sphere);
    sphereNode->transform.Translate(0,2,0);
    auto backpackNode = scene->AddChild(backpack);
    backpackNode->transform.Translate(5,4,0);
    auto sponzaNode = scene->AddChild(sponza);
    
    std::cout << "Creating renderer..." << std::endl;
    renderer = std::make_unique<Renderer::DeferredRenderer>();
    fxaa = std::make_unique<PostProcessing::Fxaa>();
    hdr = std::make_unique<PostProcessing::Hdr>();
    bloom = std::make_unique<PostProcessing::Bloom>();
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
    while (GLenum error = glGetError()) { std::cerr << "Render error: " << error << std::endl; }

    displayGui();
}
void Demo1::CleanUp() {

}

void Demo1::displayGui() {
    ImGui::Begin("Plum Engine v2.00 Beta", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
    ImGui::SetWindowPos(ImVec2(0,0));
    ImGui::TextColored(ImVec4(0.3,1,1,1), "Press ` to capture/release mouse.");
    ImGui::TextColored(ImVec4(0.3,1,1,1), "Use WASD, Shift, and Spacebar to move camera.");
    ImGui::Spacing();
    
    if (ImGui::CollapsingHeader("Render Options", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("IBL Intensity", &renderOptions.iblIntensity, 0.0f, 1.0f);
        ImGui::Checkbox("SSAO", &renderOptions.ssao); ImGui::SameLine();
        ImGui::Checkbox("FXAA", &renderOptions.fxaa);
        ImGui::Checkbox("Bloom", &renderOptions.bloom); ImGui::SameLine();
        if (ImGui::Checkbox("HDR", &renderOptions.hdr))
            ImGui::SliderFloat("HDR Exposure", &renderOptions.hdrExposure, 0.0f, 10.0f);
    }

    int i = 0;
    if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen)) {
        for (auto& node : scene->children) {
            if (ImGui::TreeNode((node->name+"##"+std::to_string(i++)).c_str())) {
                if (ImGui::TreeNode("Model Matrix")) {
                    if (ImGui::DragFloat3("Position", glm::value_ptr(node->transform.position), 0.01f, 0.0f, 0.0f, "%.2f")
                        || ImGui::DragFloat3("Scale", glm::value_ptr(node->transform.position), 0.01f, 0.0f, 100.0f, "%.3f"))
                        // ImGui::DragFloat3("Rotation", glm::value_ptr(node->transform.rotationE), 0.01, -360.0f, 360.0f, "%.1f");
                        node->transform.Update();
                    ImGui::TreePop();
                }
                if (ImGui::Button("Delete")) {
                    scene->RemoveChild(node);
                    ImGui::TreePop();
                    break;
                }
                ImGui::TreePop();
            }
        }
    }
    ImGui::End();
}