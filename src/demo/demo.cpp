#include "demo/demo.hpp"

#include "interface/widget.hpp"
#include "material/material.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

void Demo::baseSetup() {
    std::clog << "Creating renderer..." << std::endl;
    renderer = std::make_unique<Renderer::DeferredRenderer>();
    fxaa = std::make_unique<PostProcessing::Fxaa>();
    hdr = std::make_unique<PostProcessing::Hdr>();
    bloom = std::make_unique<PostProcessing::Bloom>();
    
    std::clog << "Creating environment..." << std::endl;
    environment = std::make_unique<Scene::Environment>();
}

void Demo::displayMainGui() {
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
    
    if (ImGui::CollapsingHeader("External Assets", ImGuiTreeNodeFlags_DefaultOpen)) {
        static Directory displayDir("assets");
        static const Directory highestDir("assets");
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
        scene->DisplayWidget();
    }
    ImGui::End();
}