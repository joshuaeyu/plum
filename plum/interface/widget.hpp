#pragma once

#include <imgui/imgui.h>

#include <string>
#include <vector>

class Widget {
    public:
        inline static void Combo(const char* label, const std::vector<std::string>& items, int* item_selected_idx, const std::string& preview) {
            if (ImGui::BeginCombo(label, preview.c_str())) {
                for (int i = 0; i < items.size(); i++) {
                    const bool isSelected = (*item_selected_idx == i);
                    if (ImGui::Selectable(items[i].c_str(), isSelected))
                        *item_selected_idx = i;
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }
        
        virtual void DisplayWidget() = 0;
};
