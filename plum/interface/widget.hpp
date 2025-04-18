#pragma once

#include <plum/component/component.hpp>
#include <plum/util/file.hpp>

#include <imgui/imgui.h>

#include <memory>
#include <set>
#include <string>

namespace Widget {
    // Displays frame time and frame rate.
    void PerformanceWidget(float points_per_sec = 0.5f, float seconds_to_display = 20.f);

    // Returns Path to be displayed.
    Path FileExplorerWidget(Path display_path, Path highest_path);

    // Returns Path selected.
    bool PathComboWidget(int* id, Directory directory, const char* label, const std::set<std::string>& extensions, Path* selected_path, Path default_path = Path());
    // need to 
    // - detect whether there was a change
    // - include support for an empty / default option
        // - either have a button to enable/disable the combo box
        // - or have an empty default option

    // Returns Component created, or nullptr.
    std::shared_ptr<Component::ComponentBase> ComponentCreationWidget(bool* show_widget);
    
    // Returns Material created, or nullptr.
    std::shared_ptr<Material::MaterialBase> MaterialCreationWidget(bool* show_widget);
};