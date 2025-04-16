#include <plum/interface/widget.hpp>

#include <plum/component/all.hpp>

#include <string>
#include <vector>

Path Widget::FileExplorerWidget(Path display_path, Path highest_path) {
    if (ImGui::ArrowButton("##fileexplorer_back", ImGuiDir_Left)) {
        // Don't go any higher than "assets/"
        if (display_path.RawPath() != highest_path.RawPath()) {
            display_path = display_path.Parent();
        }
    }
    ImGui::SameLine();
    ImGui::Text("Path: %s", display_path.RelativePath().c_str());

    ImGui::BeginChild("##fileexplorer_body", ImVec2(0,100), ImGuiChildFlags_ResizeY | ImGuiChildFlags_FrameStyle);
    if (display_path.IsDirectory()) {
        const Directory dir(display_path);
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
    return display_path;
}

Path Widget::PathComboWidget(int* id, Directory directory, const char* label, const std::set<std::string>& extensions) {
    static int idCounter = 0;
    static std::vector<int> allPathSelectedIdx;
    static std::vector<std::vector<Path>> allPaths;
    static std::vector<std::vector<char*>> allRelativePathNames;

    bool updateNeeded;
    if (*id < 0 || *id >= idCounter) {
        *id = idCounter++;
        allPathSelectedIdx.push_back(0);
        allPaths.emplace_back();
        allRelativePathNames.emplace_back();
        updateNeeded = true;
    } else {
        updateNeeded = directory.NeedsResync();
    }

    int& pathSelectedIdx = allPathSelectedIdx[*id];
    std::vector<Path>& paths = allPaths[*id];
    std::vector<char*>& relativePathNames = allRelativePathNames[*id];
    
    if (updateNeeded) {
        paths = directory.ListOnlyRecursive(extensions);
        relativePathNames.resize(paths.size());
        for (int i = 0; i < paths.size(); i++) {
            delete relativePathNames[i];
            relativePathNames[i] = new char[512];
            strcpy(relativePathNames[i], paths[i].RelativePath(directory.RawPath()).c_str());
        }
    }

    if (!paths.empty()) {
        ImGui::Combo(label, &pathSelectedIdx, relativePathNames.data(), relativePathNames.size());
    }

    return paths[pathSelectedIdx];
}

std::shared_ptr<Component::ComponentBase> Widget::ComponentCreationWidget(bool* show_widget) {
    std::shared_ptr<Component::ComponentBase> result;
    static const std::vector<const char*> componentTypes = {"Light", "Primitive", "Model"};
    static int componentSelectedIdx = 0;
    ImGui::Combo("Component Type", &componentSelectedIdx, componentTypes.data(), componentTypes.size());
    switch (componentSelectedIdx) {
        case 0: // Light
        {
            static const std::vector<const char*> lightTypes = {"Directional", "Point"};
            static int lightSelectedIdx = 0;
            ImGui::Combo("Light Type", &lightSelectedIdx, lightTypes.data(), lightTypes.size());
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
        }
        break;
        case 1: // Primitive
        {
            static const std::vector<const char*> primitiveTypes = {"Cube", "Sphere", "Plane"};
            static int primitiveSelectedIdx = 0;
            ImGui::Combo("Primitive Type", &primitiveSelectedIdx, primitiveTypes.data(), primitiveTypes.size());
            static int uvDims[] = {1, 1};
            ImGui::InputInt2("UV Dimensions", uvDims);
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
            int widgetId;
            File modelPath = PathComboWidget(&widgetId, modelsDir, "Model Path", Asset::modelExtensions);
            if (ImGui::Button("Create")) {
                result = std::make_shared<Component::Model>(modelPath);
                *show_widget = !*show_widget;
            }
        }
            break;
    }
    return result;
}