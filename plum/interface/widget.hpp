#pragma once

#include <plum/component/component.hpp>
#include <plum/util/file.hpp>

#include <imgui/imgui.h>

#include <memory>
#include <set>
#include <string>

namespace Interface {

    // Displays frame time and frame rate.
    void PerformanceWidget(float points_per_sec = 0.5f, float seconds_to_display = 20.f);

    // Displays items within a directory. Returns true if display_dir has changed.
    bool FileExplorerWidget(Directory* display_dir, const Directory& highest_dir);

    // Displays a combo box containing specified file types within a directory(recursively). Returns true if selected_path has changed.
    bool PathComboWidget(int* id, const Directory& directory, const char* label, const std::set<std::string>& extensions, Path* selected_path, const Path& default_path = Path());

    // Displays a button and text input widget for editing a string. Returns true if text has changed.
    bool TextEditWidget(int* id, std::string* text, const char* label = "Rename");

    // Displays a component creation widget. Returns Component created, or nullptr.
    std::shared_ptr<Component::ComponentBase> ComponentCreationWidget(bool* show_widget);
    
    // Displays a material creation widget. Returns Material created, or nullptr.
    std::shared_ptr<Material::MaterialBase> MaterialCreationWidget(bool* show_widget);
    
};