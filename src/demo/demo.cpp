#include "demo/demo.hpp"

#include "interface/widget.hpp"
#include "material/material.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

Demo::Demo(const std::string& title) 
    : title(title)
{}

void Demo::Initialize() {
    std::clog << "Creating renderer..." << std::endl;
    renderer = std::make_unique<Renderer::DeferredRenderer>();
    fxaa = std::make_unique<PostProcessing::Fxaa>();
    hdr = std::make_unique<PostProcessing::Hdr>();
    bloom = std::make_unique<PostProcessing::Bloom>();
    
    std::clog << "Creating environment..." << std::endl;
    environment = std::make_unique<Scene::Environment>();

    initialize();
}

void Demo::DisplayScene() {
    preDisplayScene();

    camera->ProcessInputs();
    renderer->ssao = renderOptions.ssao;
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

    postDisplayScene();

    fbo->BlitToDefault();
}

void Demo::DisplayGui() {
    ImGui::TextColored(ImVec4(0.3,1,1,1), "Press ` to capture/release mouse.");
    ImGui::TextColored(ImVec4(0.3,1,1,1), "Use WASD, Shift, and Spacebar to move camera.");
    ImGui::Spacing();
    
    constexpr float pointsPerSecond = 1.f;    // points/sec
    constexpr int secondsToDisplay = 20.f; // sec / points/sec
    static Interface::PerformanceWidget performanceWidget;
    performanceWidget.Display(pointsPerSecond, secondsToDisplay);

    if (ImGui::CollapsingHeader("Render Options", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("HDR", &renderOptions.hdr);
        if (renderOptions.hdr) {
            ImGui::SameLine();
            ImGui::PushItemWidth(100.f);
            ImGui::DragFloat("Exposure", &renderOptions.hdrExposure, 0.01f, 0.0f, 1000.0f);
            ImGui::PopItemWidth();
        }
        ImGui::Checkbox("SSAO", &renderOptions.ssao); ImGui::SameLine();
        ImGui::Checkbox("FXAA", &renderOptions.fxaa); ImGui::SameLine();
        ImGui::Checkbox("Bloom", &renderOptions.bloom);
    }
    
    if (ImGui::CollapsingHeader("External Assets", ImGuiTreeNodeFlags_DefaultOpen)) {
        static const Directory highestDir("assets");
        static Directory displayDir("assets");
        static Interface::FileExplorerWidget fileExplorerWidget;
        static Path selectedPath;
        if (fileExplorerWidget.Display(&displayDir, highestDir, &selectedPath)) {
            static Interface::AssetImportWidget assetImportWidget;
            static std::shared_ptr<Asset> asset;
            assetImportWidget.Display(selectedPath, &asset);
        }
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
                    for (auto& material : Material::materials) {
                        if (newName == material->name) {
                            nameExists = true;
                            newName = mat->name + "-" + std::to_string(i++);
                            break;
                        }
                    }
                }
                mat->name = newName;
                Material::materials.insert(mat);
                showMaterialCreationWidget = false;
            }
        }
        for (auto& material : Material::materials) {
            if (ImGui::TreeNode(material->name.c_str())) {
                material->DisplayWidget();
                ImGui::TreePop();
            }
        }
    }
    if (ImGui::CollapsingHeader("Environment", ImGuiTreeNodeFlags_DefaultOpen)) {
        environment->DisplayWidget();
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
        scene->DisplayWidget();
    }

    displayGui();
}

void Demo::CleanUp() {
    cleanUp();

    renderer.reset();
    fxaa.reset();
    hdr.reset();
    bloom.reset();
    
    scene.reset();
    environment.reset();
    camera.reset();
}