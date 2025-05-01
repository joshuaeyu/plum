#include <plum/interface/widget.hpp>

#include <plum/component/all.hpp>
#include <plum/util/time.hpp>

#include <imgui/imgui_stdlib.h>

#include <iostream>
#include <string>
#include <vector>

namespace Interface {

bool PerformanceWidget::Display(float points_per_sec, float seconds_to_display) {
    static std::vector<float> framerateData = {Time::FrameRate()};
    static float displayTimer = 0;

    // Plot frametime over time and display framerate
    const int numDisplayPoints = points_per_sec * seconds_to_display;
    if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("%.2f ms/frame (%.1f fps)", 1000.f/framerateData.back(), framerateData.back());
        int n = std::min(static_cast<int>(framerateData.size()), numDisplayPoints);
        ImGui::PlotLines("ms/frame", framerateData.data(), n, framerateData.size() - n, NULL, FLT_MAX, FLT_MAX, ImVec2(250,50));
    }
    
    // Update data vector at a rate of points_per_sec
    if (displayTimer >= points_per_sec) {
        framerateData.push_back(Time::FrameRate());
        if (framerateData.size() >= 200) {
            framerateData.erase(framerateData.begin(), framerateData.end() - 100);
        }
        displayTimer = 0;
    }
    displayTimer += Time::DeltaTime();

    return true;
}

bool FileExplorerWidget::Display(Directory* display_dir, const Directory& highest_dir) {
    bool result = false;

    // Back button and path text
    if (ImGui::ArrowButton("##fileexplorer_backbutton", ImGuiDir_Left)) {
        if (display_dir->RawPath() != highest_dir.RawPath()) { // Don't go any higher than "assets/"
            *display_dir = display_dir->Parent();
            result = true;
        }
    }
    ImGui::SameLine();
    ImGui::Text("Path: %s", display_dir->RelativePath().c_str());

    // Files list
    ImGui::BeginChild("##fileexplorer_body", ImVec2(0,100), ImGuiChildFlags_ResizeY | ImGuiChildFlags_FrameStyle);
    if (display_dir->IsDirectory()) {
        ImGui::Unindent(15);
        for (auto& child : display_dir->List()) {
            ImGui::TreeNodeEx(child.Filename().c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_NoTreePushOnOpen);
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered()) {
                if (child.IsDirectory()) {
                    result = true;
                    *display_dir = child;
                    break;
                }
            }
        }
        ImGui::Indent(15);
    }
    ImGui::EndChild();

    return result;
}

bool PathComboWidget::Display(const Directory& directory, const char* label, const std::set<std::string>& extensions, Path* selected_path, const Path& default_path) {
    // Update paths list and relative path names list if needed
    if (directory.NeedsResync() || firstDisplay) {
        std::vector<Path> directoryPaths = directory.ListOnlyRecursive(extensions);
        if (firstDisplay) {
            paths.resize(1 + directoryPaths.size());
            paths[0] = default_path;
            firstDisplay = false;
        }
        for (int i = 0; i < directoryPaths.size(); i++) {
            paths[i+1] = directoryPaths[i];
            if (selected_path->RawPath() == paths[i+1].RawPath()) {
                pathSelectedIdx = i+1;
            }
        }
        relativePathNames.resize(paths.size());
        relativePathNames[0] = new char[512];
        if (default_path.IsEmpty()) {
            strcpy(relativePathNames[0], "");
        } else {
            strcpy(relativePathNames[0], paths[0].RelativePath(directory.RawPath()).c_str());
        }
        for (int i = 1; i < paths.size(); i++) {
            delete relativePathNames[i];
            relativePathNames[i] = new char[512];
            strcpy(relativePathNames[i], paths[i].RelativePath(directory.RawPath()).c_str());
        }
    }

    // Update selected path if index has changed    
    int idxBefore = pathSelectedIdx;
    if (!paths.empty()) {
        ImGui::Combo(label, &pathSelectedIdx, relativePathNames.data(), relativePathNames.size());
    }
    if (idxBefore != pathSelectedIdx) {
        *selected_path = paths[pathSelectedIdx];
        return true;
    } else {
        return false;
    }
}

bool TextEditWidget::Display(std::string* text, const char* label) {    
    // Render buttons and text input field
    if (!showInput) {
        if (ImGui::Button(label)) {
            input = *text;
            showInput = true;
        }
    } else {
        if (ImGui::Button("Cancel")) {
            showInput = false;
        }
        ImGui::SameLine();
        if (ImGui::InputText("Name", &input, ImGuiInputTextFlags_EnterReturnsTrue)) {
            *text = input;
            return true;
        }
    }
    return false;
}

bool ComponentCreationWidget::Display(std::shared_ptr<Component::ComponentBase>* component) {
    static const std::vector<const char*> componentTypes = {"Light", "Primitive", "Model"};

    const std::string strId = std::string("##componentcreation") + std::to_string(widgetId);
    if (!ImGui::BeginChild(strId.c_str(), ImVec2(0,0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY)) {
        ImGui::EndChild();
        return false;
    }

    ImGui::Combo("Component Type", &componentSelectedIdx, componentTypes.data(), componentTypes.size());
    ImGui::Spacing();
    
    bool result = false;
    switch (componentSelectedIdx) {
        case 0: // Light
        {
            static const std::vector<const char*> lightTypes = {"Directional", "Point"};
            ImGui::Combo("Light Type", &lightSelectedIdx, lightTypes.data(), lightTypes.size());
            ImGui::Spacing();
            if (ImGui::Button("Create")) {
                switch (lightSelectedIdx) {
                    case 0: // Directional
                        *component = std::make_shared<Component::DirectionalLight>();
                        break;
                    case 1: // Point
                        *component = std::make_shared<Component::PointLight>();
                        break;
                }
                result = true;
            }
            break;
        }
        case 1: // Primitive
        {
            static const std::vector<const char*> primitiveTypes = {"Cube", "Sphere", "Plane"};
            ImGui::Combo("Primitive Type", &primitiveSelectedIdx, primitiveTypes.data(), primitiveTypes.size());
            static int uvDims[] = {1, 1};
            ImGui::InputInt2("UV Dimensions", uvDims);
            ImGui::Spacing();
            if (ImGui::Button("Create")) {
                switch (primitiveSelectedIdx) {
                    case 0: // Cube
                        *component = std::make_shared<Component::Cube>(uvDims[0], uvDims[1]);
                        break;
                    case 1: // Sphere
                        *component = std::make_shared<Component::Sphere>(uvDims[0], uvDims[1]);
                        break;
                    case 2: // Plane
                        *component = std::make_shared<Component::Plane>(uvDims[0], uvDims[1]);
                        break;
                }
                result = true;
            }
        }
            break;
        case 2: // Model
        {
            static const Directory modelsDir("assets/models");
            pathComboWidget.Display(modelsDir, "Model Path", AssetUtils::modelExtensions, &modelPathSelected, Path());
            ImGui::Spacing();
            if (ImGui::Button("Create")) {
                auto model = AssetManager::Instance().LoadHot<ModelAsset>(modelPathSelected);
                *component = std::make_shared<Component::Model>(model);
                result = true;
            }
            break;
        }
    }
    
    ImGui::EndChild();
    return result;
}

bool MaterialCreationWidget::Display(std::shared_ptr<Material::MaterialBase>* material) {
    static const std::vector<const char*> materialTypes = {"PBR Metallic"};

    const std::string strId = std::string("##materialcreation") + std::to_string(widgetId);
    if (!ImGui::BeginChild(strId.c_str(), ImVec2(0,0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY)) {
        ImGui::EndChild();
        return false;
    }
    
    ImGui::Combo("Material Type", &materialSelectedIdx, materialTypes.data(), materialTypes.size());
    ImGui::Spacing();
    
    bool result = false;
    switch (materialSelectedIdx) {
        case 0: // PBR Metallic
        {
            ImGui::Spacing();
            if (ImGui::Button("Create")) {
                *material = std::static_pointer_cast<Material::MaterialBase>(std::make_shared<Material::PBRMetallicMaterial>());
                result = true;
            }
            break;
        }
    }
    
    ImGui::EndChild();
    return result;
}

}