#include <plum/context/window.hpp>

// #include <imgui/imgui.h>
// #include <imgui/imgui_impl_glfw.h>
// #include <imgui/imgui_impl_opengl3.h>

#include <memory>
#include <iostream>
#include <exception>

namespace Context {

    WindowCreator::WindowCreator() 
    {
        // Default hints
        hints[GLFW_CONTEXT_VERSION_MAJOR]    = 4;
        hints[GLFW_CONTEXT_VERSION_MINOR]    = 1;
        hints[GLFW_OPENGL_PROFILE]           = GLFW_OPENGL_CORE_PROFILE;
        hints[GLFW_OPENGL_FORWARD_COMPAT]    = GLFW_TRUE;
        hints[GLFW_SCALE_TO_MONITOR]         = GLFW_FALSE;
        hints[GLFW_COCOA_RETINA_FRAMEBUFFER] = GLFW_FALSE;
    }
    
    std::shared_ptr<Window> WindowCreator::Create() {
        for (const auto [hint, value] : hints) {
            glfwWindowHint(hint, value);
        }
        
        GLFWwindow *window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if (window == nullptr) {
            std::cerr << "glfwCreateWindow failed" << std::endl;
            glfwTerminate();
            exit(-1);
        }
        
        return std::make_shared<Window>(window, width, height, title);
    }

    Window::Window(GLFWwindow *window, int width, int height, std::string title) 
        : handle(window),
        width(width),
        height(height),
        title(title),
        eventListener(WindowInputsAndEventsManager::CreateEventListener())
    {
        std::function<void(int,int,int,int)> staticFunc = std::bind(&Window::keyCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);   
        eventListener->SetKeyCallback(staticFunc);
    }
    Window::~Window()
    {
        // std::cout << "calling Window destructor" << std::endl;
    }

    void Window::SetTitle(const char* title) {
        glfwSetWindowTitle(handle, title);
        this->title = title;
    }
    void Window::SetWindowSize(int width, int height) {
        glfwSetWindowSize(handle, width, height);
        this->width = width;
        this->height = height;
    }

    void Window::SetInputMode(int mode, int value) {
        glfwSetInputMode(handle, mode, value);
    }

    bool Window::ShouldClose() const {
        return glfwWindowShouldClose(handle);
    }

    void Window::SwapBuffers() {
        glfwSwapBuffers(handle);
    }

    void Window::MakeCurrent()  {
        glfwMakeContextCurrent(handle);
    }

    void Window::keyCallback(int key, int scancode, int action, int mods) {
        if (action != GLFW_PRESS)
            return;

        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(handle, GLFW_TRUE);
                break;
            case GLFW_KEY_GRAVE_ACCENT:
                int inputmode = glfwGetInputMode(handle, GLFW_CURSOR);
                if (inputmode == GLFW_CURSOR_NORMAL) {
                    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    // firstMouse = true;
                } else if (inputmode == GLFW_CURSOR_DISABLED) {
                    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
                break;
        }
    }

    // Gui::Gui(GLFWwindow *window) {
    //     IMGUI_CHECKVERSION();
    //     ImGui::CreateContext();
    //     ImGuiIO& io = ImGui::GetIO();
    //     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //     ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    //     ImGui_ImplOpenGL3_Init();
    // }

}