#include <plum/interface/widget.hpp>

#include <plum/component/all.hpp>
#include <plum/util/time.hpp>

#include <imgui/imgui_stdlib.h>

#include <iostream>
#include <string>
#include <vector>

void Interface::PerformanceWidget(float points_per_sec, float seconds_to_display) {
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
}

bool Interface::FileExplorerWidget(Directory* display_dir, const Directory& highest_dir) {
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
    }
    ImGui::EndChild();

    return result;
}

bool Interface::PathComboWidget(int* id, const Directory& directory, const char* label, const std::set<std::string>& extensions, Path* selected_path, const Path& default_path) {
    static int idCounter = 0;
    static std::vector<int> allPathSelectedIdx;
    static std::vector<std::vector<Path>> allPaths;
    static std::vector<std::vector<char*>> allRelativePathNames;

    // Handle new ids and determine if paths list should be requeried
    bool updateNeeded, newId;
    if (*id < 0 || *id >= idCounter) {
        *id = idCounter++;
        std::clog << *id << " " << std::endl;
        allPathSelectedIdx.push_back(0);
        allPaths.emplace_back();
        allRelativePathNames.emplace_back();
        updateNeeded = true;
        newId = true;
    } else {
        updateNeeded = directory.NeedsResync();
        newId = false;
    }

    // Store references to data relevant to current id
    int& pathSelectedIdx = allPathSelectedIdx[*id];
    std::vector<Path>& paths = allPaths[*id];
    std::vector<char*>& relativePathNames = allRelativePathNames[*id];
    
    // Update paths list and relative path names list if needed
    if (updateNeeded) {
        std::vector<Path> directoryPaths = directory.ListOnlyRecursive(extensions);
        if (newId) {
            paths.resize(1 + directoryPaths.size());
            paths[0] = default_path;
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
            relativePathNames[i] = new char[512];   // leaks
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

bool Interface::TextEditWidget(int* id, std::string* text, const char* label) {
    static int idCounter = 0;
    static std::vector<std::string> allInput;
    static std::vector<bool> allShowInput;
    static bool showTextInput = false;
    
    // Handle new ids
    bool newId = false;
    if (*id < 0 || *id >= idCounter) {
        *id = idCounter++;
        std::clog << *id << " " << std::endl;
        allInput.emplace_back(*text);
        allShowInput.push_back(false);
    }
    
    // Store reference to data relevant to current id
    std::string& input = allInput[*id];

    // Render buttons and text input field
    if (!allShowInput[*id]) {
        if (ImGui::Button(label)) {
            input = *text;
            allShowInput[*id] = true;
        }
    } else {
        if (ImGui::Button("Cancel")) {
            allShowInput[*id] = false;
        }
        ImGui::SameLine();
        if (ImGui::InputText("Name", &input, ImGuiInputTextFlags_EnterReturnsTrue)) {
            *text = input;
            return true;
        }
    }
    return false;
}

std::shared_ptr<Component::ComponentBase> Interface::ComponentCreationWidget(bool* show_widget) {
    if (!ImGui::BeginChild("##componentcreation", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY)) {
        ImGui::EndChild();
        return nullptr;
    }

    std::shared_ptr<Component::ComponentBase> result;
    static const std::vector<const char*> componentTypes = {"Light", "Primitive", "Model"};
    static int componentSelectedIdx = 0;
    ImGui::Combo("Component Type", &componentSelectedIdx, componentTypes.data(), componentTypes.size());
    ImGui::Spacing();

    switch (componentSelectedIdx) {
        case 0: // Light
        {
            static const std::vector<const char*> lightTypes = {"Directional", "Point"};
            static int lightSelectedIdx = 0;
            ImGui::Combo("Light Type", &lightSelectedIdx, lightTypes.data(), lightTypes.size());
            ImGui::Spacing();
            if (ImGui::Button("Create")) {
                switch (lightSelectedIdx) {
                    case 0: // Directional
                        result = std::make_shared<Component::DirectionalLight>();
                        break;
                    case 1: // Point
                        result = std::make_shared<Component::PointLight>();
                        break;
                }
                *show_widget = !*show_widget;
            }
            break;
        }
        case 1: // Primitive
        {
            static const std::vector<const char*> primitiveTypes = {"Cube", "Sphere", "Plane"};
            static int primitiveSelectedIdx = 0;
            ImGui::Combo("Primitive Type", &primitiveSelectedIdx, primitiveTypes.data(), primitiveTypes.size());
            static int uvDims[] = {1, 1};
            ImGui::InputInt2("UV Dimensions", uvDims);
            ImGui::Spacing();
            if (ImGui::Button("Create")) {
                switch (primitiveSelectedIdx) {
                    case 0: // Cube
                        result = std::make_shared<Component::Cube>(uvDims[0], uvDims[1]);
                        break;
                    case 1: // Sphere
                        result = std::make_shared<Component::Sphere>(uvDims[0], uvDims[1]);
                        break;
                    case 2: // Plane
                        result = std::make_shared<Component::Plane>(uvDims[0], uvDims[1]);
                        break;
                }
                *show_widget = !*show_widget;
            }
        }
            break;
        case 2: // Model
        {
            static const Directory modelsDir("assets/models");
            static Path modelPath = Path();
            static int widgetId;
            PathComboWidget(&widgetId, modelsDir, "Model Path", AssetUtils::modelExtensions, &modelPath, Path());
            ImGui::Spacing();
            if (ImGui::Button("Create")) {
                auto model = AssetManager::Instance().LoadHot<ModelAsset>(modelPath);
                result = std::make_shared<Component::Model>(model);
                *show_widget = !*show_widget;
            }
            break;
        }
    }
    ImGui::EndChild();
    return result;
}

std::shared_ptr<Material::MaterialBase> Interface::MaterialCreationWidget(bool* show_widget) {
    if (!ImGui::BeginChild("##materialcreation", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY)) {
        ImGui::EndChild();
        return nullptr;
    }

    std::shared_ptr<Material::MaterialBase> result;
    static const std::vector<const char*> materialTypes = {"PBR Metallic"};
    static int materialSelectedIdx = 0;
    ImGui::Combo("Material Type", &materialSelectedIdx, materialTypes.data(), materialTypes.size());
    ImGui::Spacing();
    
    switch (materialSelectedIdx) {
        case 0: // PBR Metallic
        {
            ImGui::Spacing();
            if (ImGui::Button("Create")) {
                result = std::make_shared<Material::PBRMetallicMaterial>();
                *show_widget = !*show_widget;
            }
            break;
        }
    }
    ImGui::EndChild();
    return result;
}