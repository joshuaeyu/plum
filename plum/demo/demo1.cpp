#include "demo1.hpp"

#include <plum/context/asset.hpp>
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
    // auto backpackNode = scene->AddChild(backpack);
    // backpackNode->transform.Translate(5,4,0);
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
    scene.reset();
    environment.reset();
    camera.reset();
    renderer.reset();
    fxaa.reset();
    hdr.reset();
    bloom.reset();
}

void Demo1::displayGui() {
    ImGui::Begin("Plum Engine v2.00 Beta", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
    ImGui::SetWindowPos(ImVec2(0,0));
    ImGui::TextColored(ImVec4(0.3,1,1,1), "Press ` to capture/release mouse.");
    ImGui::TextColored(ImVec4(0.3,1,1,1), "Use WASD, Shift, and Spacebar to move camera.");
    ImGui::Spacing();
    
    constexpr float displayCooldown = 0.5f;    // points/sec
    constexpr int numDisplayPoints = 20 / displayCooldown; // sec / points/sec
    static std::vector<float> framerateData = {Time::FrameRate()};
    static float displayTimer = 0;
    if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("%.2f ms/frame (%.1f fps)", 1000.f/framerateData.back(), framerateData.back());
        int n = std::min(static_cast<int>(framerateData.size()), numDisplayPoints);
        ImGui::PlotLines("ms/frame", framerateData.data(), n, framerateData.size() - n, NULL, FLT_MAX, FLT_MAX, ImVec2(250,50));
    }
    if ((displayTimer += Time::DeltaTime()) >= displayCooldown) {
        framerateData.push_back(Time::FrameRate());
        if (framerateData.size() >= 200) {
            framerateData.erase(framerateData.begin(), framerateData.end() - 100);
        }
        displayTimer = 0;
    }

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
        static Path displayPath = "assets";
        if (ImGui::ArrowButton("##back", ImGuiDir_Left)) {
            if (displayPath.RawPath() != fs::current_path() / "assets") {
                displayPath = displayPath.Parent();
            }
        }
        ImGui::SameLine();
        ImGui::Text("Path: %s", displayPath.RelativePath().c_str());
        displayPath = gui_DisplayFilePath(displayPath);
    }

    if (ImGui::CollapsingHeader("Assets", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("Import Texture")) {
        }
        ImGui::SameLine();
        if (ImGui::Button("Import Model")) {
        }
        ImGui::SameLine();
        if (ImGui::Button("New Material")) {
        }
        if (ImGui::TreeNode("Textures")) {
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Models")) {
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Materials")) {
            ImGui::TreePop();
        }
    }
    if (ImGui::CollapsingHeader("Environment", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::TreeNode("Skybox")) {
            static bool showChild = false;
            if (ImGui::Button("Edit")) {
                showChild = !showChild;
            }
            if (showChild) {
                static int sel = 0;
                ImGui::RadioButton("Equirectangular", &sel, 0);
                ImGui::RadioButton("Six Faces", &sel, 1);
            }
            // std::string preview = environment->skybox->
            // if (ImGui::BeginCombo("Diffuse Texture", preview.c_str())) {
            //     for (auto it = resources->Textures.begin(); it != resources->Textures.end(); it++) {
            //         if (it->second->Type != Tex::Tex_Type::TEX_DIFFUSE)
            //             continue;
            //         bool isSelected = (_skyboxSelectionStr == it->first); 
            //         if (ImGui::Selectable(it->first.c_str(), isSelected)) {
            //             _skyboxSelectionStr = it->first;
            //             scene->EnvironmentMap = it->second;
            //             engine->InitEnvironment(scene->EnvironmentMap);
            //             ImGui::SetItemDefaultFocus();
            //         }
            //     }
            //     ImGui::EndCombo();
            // }
            ImGui::TreePop();
        }
    }
    int i = 0;
    if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("New Empty Node")) {
        }
        ImGui::SameLine();
        if (ImGui::Button("New Component Node")) {
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

Path Demo1::gui_DisplayFilePath(Path path) {
    ImGui::BeginChild("fileexplorer", ImVec2(0,100), ImGuiChildFlags_ResizeY | ImGuiChildFlags_FrameStyle);
    if (path.IsDirectory()) {
        const Directory dir(path);
        for (auto& child : dir.List()) {
            ImGui::TreeNodeEx(child.Filename().c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_NoTreePushOnOpen);
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered()) {
                if (child.IsDirectory()) {
                    ImGui::EndChild();
                    return child;
                }
            }
        }
    }
    ImGui::EndChild();
    return path;
    
    // const bool isDirectory = path.IsDirectory();
    // ImGuiTreeNodeFlags flags = isDirectory ? ImGuiTreeNodeFlags_None : ImGuiTreeNodeFlags_Leaf;
    // bool expanded = ImGui::TreeNodeEx(path.Filename().c_str(), flags);
    // // Right-click context menu
    // if (!isDirectory && ImGui::BeginPopupContextItem()) {
    //     std::string assetType = Asset::extensionTable.at(path.Extension());
    //     // if (ImGui::Button(("Import "+assetType).c_str())) {
    //     //     if (assetType == "Texture") {
    //     //         Asset::AssetManager::Instance().ImportAsset<Material::Texture>(path, false, )
    //     //     } else if (assetType == "Model") {

    //     //     }
    //     // }
    //     ImGui::EndPopup();
    // }
    // if (isDirectory && expanded) {
    //     const Directory dir(path);
    //     std::vector<Path> children = dir.List();
    //     for (auto& child : children) {
    //         gui_DisplayFilePath(child);
    //     }
    // }
    // if (expanded) {
    //     ImGui::TreePop();
    // }
}

bool Demo1::gui_DisplaySceneNode(Scene::SceneNode& node, int& i) {
    bool expanded = ImGui::TreeNodeEx((node.name+"##"+std::to_string(i++)).c_str());
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
        if (ImGui::TreeNodeEx("[Transform]")) {
            bool pos = ImGui::DragFloat3("Position", glm::value_ptr(node.transform.position), 0.01f, 0.0f, 0.0f, "%.2f");
            bool rot = ImGui::DragFloat3("Rotation", glm::value_ptr(node.transform.rotationEuler), 0.1f, 0.0f, 0.0f, "%.1f");
            bool scale = ImGui::DragFloat3("Scale", glm::value_ptr(node.transform.scale), 0.001f, 0.001f, 1e6f, "%.3f");
            if (pos || scale || rot)
                node.transform.Update();
            ImGui::TreePop();
        }
        for (auto& child : node.children) {
            if (!gui_DisplaySceneNode(*child, i))
                node.RemoveChild(child);
        }
        ImGui::TreePop();
    }
    return true;
}