#include <plum/interface/interface.hpp>

#include <plum/context/application.hpp>

Interface::Interface() 
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    ImGui_ImplGlfw_InitForOpenGL(Context::Application::Instance().activeWindow->Handle(), true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

Interface::~Interface() {

}

void Interface::Predisplay() {
    setStyle(ImGui::GetStyle());
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
void Interface::Display() {
    showInterface();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Interface::setStyle(ImGuiStyle& style) {
    float r = 103.f/255.f;
    float g = 49.f/255.f;
    float b = 71.f/255.f;
    float a = 0.5f;
    
    float r_hov = r*1.5f;
    float g_hov = g*1.5f;
    float b_hov = b*1.5f;

    ImGui::StyleColorsClassic(&style);
    
    style.FrameRounding = 1.f;
    style.ChildRounding = 1.f;
    style.WindowRounding = 1.f;

    style.Colors[ImGuiCol_TitleBg]              = ImVec4(r,g,b,a);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(r,g,b,a+0.25);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0,0,0,a);
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
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.25,0.25,0.25,0.6);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(r,g,b,0.5);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(r,g,b,0.7);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(r,g,b,0.8);
    style.Colors[ImGuiCol_NavHighlight]         = ImVec4(r_hov*1.5, g_hov*1.5, b_hov*1.5, a);
}

void Interface::showInterface() {
    ImGui::Begin("Plum Engine v2.00 Beta", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
    ImGui::SetWindowPos(ImVec2(0,0));
    ImGui::TextColored(ImVec4(0.3,1,1,1), "Press ` to capture/release mouse.");
    ImGui::TextColored(ImVec4(0.3,1,1,1), "Use WASD, Shift, and Spacebar to move camera.");
    ImGui::Spacing();
    
    if (ImGui::CollapsingHeader("Render Options", ImGuiTreeNodeFlags_DefaultOpen)) {
        // ImGui::SliderFloat("IBL", &RenderOptions.ibl, 0.0f, 1.0f);
        // ImGui::Checkbox("SSAO", &RenderOptions.ssao); 
        // ImGui::SameLine();
        ImGui::Checkbox("FXAA", &RenderOptions.fxaa);
        ImGui::Checkbox("HDR", &RenderOptions.hdr); 
        // ImGui::SameLine();
        ImGui::Checkbox("Bloom", &RenderOptions.bloom);
        // if (RenderOptions.hdr)
        //     ImGui::SliderFloat("HDR Exposure", &HdrExposure, 0.0, 5.0);
    }

    ImGui::End();
}