#pragma once

#include <plum/component/component.hpp>
#include <plum/util/file.hpp>

#include <imgui/imgui.h>

#include <memory>
#include <set>
#include <string>

namespace Widget {
    // Returns Path to be displayed.
    Path FileExplorerWidget(Path display_path, Path highest_path);

    // Returns Path selected.
    Path PathComboWidget(int* id, Directory directory, const char* label, const std::set<std::string>& extensions);

    // Returns Component created, or nullptr.
    std::shared_ptr<Component::ComponentBase> ComponentCreationWidget(bool* show_widget);
};