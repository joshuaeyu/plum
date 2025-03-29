#include <plum/context/application.hpp>

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
        if (!glfwInit()) {
            std::cerr << "glfwInit failed" << std::endl;
            exit(-1);
        }
        
        WindowCreator creator;
        defaultWindow = creator.Create();
        defaultWindow->MakeCurrent();
        activeWindow = defaultWindow;

        WindowInputsAndEventsManager::Setup(*defaultWindow);
        currentTime = glfwGetTime();
        lastTime = currentTime;

        if (!gladLoadGL(glfwGetProcAddress)) {
            std::cerr << "gladLoadGLLoader failed" << std::endl;
            glfwTerminate();
            exit(-1);
        }

        initialize();
    }

    void Application::initialize() {
        EnableDepth();
        EnableCull();
        // EnableFramebufferSrgb();
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // Doesn't change

        GLint n;
        glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &n); // 2048
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &n);  // 16
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &n); // 80
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &n); // 65536
        glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &n); // 16384
        glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &n); // 2048
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &n); // 16384
    }

    void Application::PollInputsAndEvents() {
        WindowInputsAndEventsManager::PerFrameRoutine();
        glfwPollEvents();
        lastTime = currentTime;
        currentTime = glfwGetTime();
    }

    float Application::CurrentTime() {
        return lastTime;
    }

    float Application::DeltaTime() {
        return currentTime - lastTime;
    }

    void Application::EnableDepth(GLenum func) {
        // Default LEQUAL for skybox optimization, since depth buffer is cleared to 1.0 by default
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(func);
    }
    void Application::EnableCull(GLenum mode) {
        glEnable(GL_CULL_FACE);
        glCullFace(mode);
    }
    void Application::EnableFramebufferSrgb() {
        glEnable(GL_FRAMEBUFFER_SRGB);
    }

    void Application::DisableDepth() {
        glDisable(GL_DEPTH_TEST);
    }
    void Application::DisableCull() {
        glDisable(GL_CULL_FACE);
    }
    void Application::DisableFramebufferSrgb() {
        glDisable(GL_FRAMEBUFFER_SRGB);
    }
    
    // void ClearColor() {}
    // void ClearDepth() {}
    // void ClearStencil() {}
    
    // void SetViewport() {}
    // void UseProgram() {}
}