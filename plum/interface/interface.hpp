#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>

class Interface {
    // time
    // framerate

    public:
        Interface();
        // Rule of five
        ~Interface();
        Interface(const Interface& other) = delete;
        Interface(Interface&& other) = delete;
        Interface& operator=(const Interface& other) = delete;
        Interface& operator=(Interface&& other) = delete;
        
        void Predisplay();
        void Display();

        struct RenderOptions {
            float ibl = 1.0f;
            bool ssao = true;
            bool fxaa = true;
            bool hdr = true;
            bool bloom = true;
        } RenderOptions;

    private:
        void setStyle(ImGuiStyle& style);
        void showInterface();
        // needs: 
        //  scene, environment, camera
        //  textures, models, shapes

};