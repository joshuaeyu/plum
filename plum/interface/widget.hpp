#pragma once

#include <imgui/imgui.h>

class Widget {
    public:
        static void Combo(const std::vector<std::string>& items, int* item_selected_idx, const std::string& preview);
        
        virtual void DisplayWidget() = 0;
};

void Widget::Combo(const std::vector<std::string>& items, int* item_selected_idx, const std::string& preview) {
    if (ImGui::BeginCombo("##widgetcombo", preview.c_str())) {
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
