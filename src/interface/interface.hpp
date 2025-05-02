#pragma once

#include "context/window.hpp"

#include <imgui/imgui.h>

namespace Interface {

    void Initialize(Context::Window& window);
    void Predisplay();
    void Display();

    void setStyle(ImGuiStyle& style);

};