#pragma once

#include <plum/context/window.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>

class Interface {
    public:
        static void Initialize(Context::Window& window);
        static void Predisplay();
        static void Display();
    private:
        static void setStyle(ImGuiStyle& style);
};