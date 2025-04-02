#pragma once

#include <plum/context/window.hpp>

#include <imgui/imgui.h>

class Interface {
    public:
        static void Initialize(Context::Window& window);
        static void Predisplay();
        static void Display();
    private:
        static void setStyle(ImGuiStyle& style);
};