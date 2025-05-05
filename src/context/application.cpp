#include "context/application.hpp"

#include "demo/demo.hpp"
#include "interface/interface.hpp"
#include "asset/manager.hpp"
#include "util/time.hpp"

#include <iostream>

namespace Context {
    
    Application::~Application() {
        // std::clog << "destroying Application" << std::endl;
        glfwTerminate();
    }

    Application& Application::Instance() {
        static Application instance;
        return instance;
    }
    
    void Application::Run() {
        if (!activeDemo) {
            if (demos.empty()) {
                throw std::runtime_error("No demos were provided to run!");
            }
            activeDemo = demos[0];
        }

        while (!activeWindow->ShouldClose()) {
            activeDemo->Initialize();
            while (!activeWindow->ShouldClose() && !activeDemo->shouldEnd) {
                predisplay();
                display();
                postdisplay();
            }
            activeDemo->CleanUp();
            activeDemo = requestedDemo;
        }
    }

    Application::Application() {   
        WindowCreator creator;
        activeWindow = creator.Create();
        activeWindow->MakeCurrent();
        activeWindow->SetTitle("Plum Engine v2.0");
        activeWindow->SetWindowSize(1200, 800);

        if (!gladLoadGL(glfwGetProcAddress)) {
            glfwTerminate();
            throw std::runtime_error("gladLoadGLLoader failed!");
        }
        
        InputsAndEventsManager::Setup(activeWindow.get());
        Time::Update();
        Interface::Initialize(*activeWindow);
        
        GLint n;
        glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &n); // 2048
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &n);  // 16
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &n); // 80
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &n); // 65536
        glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &n); // 16384
        glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &n); // 2048
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &n); // 16384
    }

    void Application::predisplay() {
        InputsAndEventsManager::PollEvents();
        Time::Update();
        static float syncCooldown = 1.f;
        if ((syncCooldown -= Time::DeltaTime()) <= 0.f) {
            AssetManager::Instance().HotSyncWithDevice();
            syncCooldown = 1.f;
        }
        Interface::BeginFrame();
    }

    void Application::display() {
        activeDemo->DisplayScene();
        
        if (guiHeader()) {
            activeDemo->DisplayGui();
            guiFooter();
        }
        Interface::RenderFrame();
    }
    
    void Application::postdisplay() {
        activeWindow->SwapBuffers();
    }

    bool Application::guiHeader() {
        if (!ImGui::Begin("Plum Engine v2.00 Beta", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::End();
            return false;
        }
        ImGui::SetWindowPos(ImVec2(0,0));

        if (ImGui::BeginMenuBar()) {

            if (ImGui::BeginMenu("File")) {
                if (ImGui::BeginMenu("Open Demo")) {
                    for (const auto& demo : demos) {
                        if (ImGui::MenuItem(demo->title.c_str()) && demo != activeDemo) {
                            requestedDemo = demo;
                            requestedDemo->shouldEnd = false;
                            activeDemo->shouldEnd = true;
                        }
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Quit")) {
                    activeWindow->RequestClose();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        return true;
    }

    void Application::guiFooter() {
        ImGui::End();
    }
}