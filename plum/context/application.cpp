#include <plum/context/application.hpp>

#include <plum/demo/demo.hpp>
#include <plum/interface/interface.hpp>
#include <plum/context/time.hpp>

#include <iostream>

namespace Context {

    Application& Application::Instance() {
        static Application instance;
        return instance;
    }
    
    Application::~Application() {
        // std::cout << "calling App destructor" << std::endl;
        glfwTerminate();
    }

    Application::Application() 
    {   
        WindowCreator creator;
        activeWindow = creator.Create();
        activeWindow->MakeCurrent();
        activeWindow->SetTitle("Plum Engine v2.0");
        activeWindow->SetWindowSize(1200, 800);

        if (!gladLoadGL(glfwGetProcAddress)) {
            std::cerr << "gladLoadGLLoader failed" << std::endl;
            glfwTerminate();
            exit(-1);
        }
        
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
    
    void Application::Run() {
        if (!activeDemo) {
            std::cerr << "Demo not set!" << std::endl;
            exit(-1);
        }

        while (!activeWindow->ShouldClose()) {
            activeDemo->Initialize();
            while (!activeWindow->ShouldClose() && !activeDemo->ShouldEnd()) {
                predisplay();
                display();
                postdisplay();
            }
            activeDemo->CleanUp();
        }
    }

    void Application::predisplay() {
        InputsAndEventsManager::PollEvents();
        Time::Update();
        Interface::Predisplay();
    }

    void Application::display() {
        activeDemo->Display();
        Interface::Display();
    }
    
    void Application::postdisplay() {
        activeWindow->SwapBuffers();
    }

}