#pragma once

#include "util/file.hpp"
#include "asset/asset.hpp"

#include <imgui/imgui.h>

#include <memory>
#include <set>
#include <string>

namespace Component {
    class ComponentBase;
};

namespace Material {
    class MaterialBase;
};

namespace Interface {

    class Widget {
        public:
            Widget() : id(idCounter++), idString(std::to_string(id)) {}
            const int id;
            const std::string idString;
        private:
            inline static int idCounter = 0;
    };

    class PerformanceWidget : public Widget {
        public:
            PerformanceWidget() = default;
            // Displays frame time and frame rate.
            bool Display(float points_per_sec = 0.5f, float seconds_to_display = 20.f);
        private:
    };
    
    class FileExplorerWidget : public Widget {
        public:
            FileExplorerWidget() = default;
            // Displays items within a directory. Returns true if display_dir has changed.
            bool Display(Directory* display_dir, const Directory& highest_dir, Path* selected_path);
        private:
            std::vector<Path> items;
            std::vector<std::string> itemNames;
            Directory lastDir = Directory("");
        };
        
    class AssetImportWidget : public Widget {
        public:
            AssetImportWidget() = default;
            // Displays items within a directory. Returns true if display_dir has changed.
            bool Display(const Path& path, std::shared_ptr<Asset>* asset);
        private:
            bool imageFlip;
            bool modelFlip;
            float modelScale;
            Path lastPath;

    };

    class PathComboWidget : public Widget {
        public:
            PathComboWidget() = default;
            ~PathComboWidget() { 
                for (auto& pathName : relativePathNames) {
                    delete pathName;
                }
            }
            // Displays a combo box containing specified file types within a directory(recursively). Returns true if selected_path has changed.
            bool Display(const Directory& directory, const char* label, const std::set<std::string>& extensions, Path* selected_path, const Path& default_path = Path());
        private:
            bool firstDisplay = true;
            int pathSelectedIdx;
            std::vector<Path> paths;
            std::vector<char*> relativePathNames;   
    };

    class TextEditWidget : public Widget {
        public:
            TextEditWidget() = default;
            // Displays a button and text input widget for editing a string. Returns true if text has changed.
            bool Display(std::string* text, const char* label = "Rename");
        private:
            std::string input;
            bool showInput;
    };

    class ComponentCreationWidget : public Widget {
        public:
            ComponentCreationWidget() = default;
            // Displays a component creation widget. Returns Component created, or nullptr.
            bool Display(std::shared_ptr<Component::ComponentBase>* component);
        private:
            int componentSelectedIdx;
            int lightSelectedIdx;
            int primitiveSelectedIdx;
            Path modelPathSelected;
            PathComboWidget pathComboWidget;
    };

    class MaterialCreationWidget : public Widget {
        public:
            MaterialCreationWidget() = default;
            // Displays a material creation widget. Returns Material created, or nullptr.
            bool Display(std::shared_ptr<Material::MaterialBase>* material);
        private:
            int materialSelectedIdx;
    };

};