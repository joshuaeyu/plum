#include <plum/interface/interface.hpp>

#include <plum/scene/scene.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void Interface::Initialize(Context::Window& window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    ImGui_ImplGlfw_InitForOpenGL(window.Handle(), true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

void Interface::Predisplay() {
    setStyle(ImGui::GetStyle());
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Interface::Display() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Interface::setStyle(ImGuiStyle& style) {
    float r = 103.f/255.f;
    float g = 49.f/255.f;
    float b = 71.f/255.f;
    float a = 0.75f;
    
    float r_hov = r*1.5f;
    float g_hov = g*1.5f;
    float b_hov = b*1.5f;

    ImGui::StyleColorsClassic(&style);
    
    style.FrameRounding = 1.f;
    style.ChildRounding = 1.f;
    style.WindowRounding = 1.f;

    style.Colors[ImGuiCol_TitleBg]              = ImVec4(r,g,b,a);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(r,g,b,a+0.25);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0,0,0,0.5);
    style.Colors[ImGuiCol_Header]               = ImVec4(r*1.2,g*1.2,b*1.2,a+0.15);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(r_hov, g_hov, b_hov, a+0.15);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(r_hov, g_hov, b_hov, a+0.15);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(0.5, 0.5, 0.5, a/2);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(r_hov, g_hov, b_hov, a/1.5);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(r_hov, g_hov, b_hov, a/1.5);
    style.Colors[ImGuiCol_Button]               = ImVec4(r,g,b,a);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(r_hov, g_hov, b_hov, a);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(r_hov, g_hov, b_hov, a);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(r, g, b, a);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(1,1,1,0.6);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.25,0.25,0.25,0.5);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(r,g,b,0.5);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(r,g,b,0.7);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(r,g,b,0.8);
    style.Colors[ImGuiCol_NavHighlight]         = ImVec4(r_hov*1.5, g_hov*1.5, b_hov*1.5, a);
}