#include "context/window.hpp"

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
    
    std::shared_ptr<Window> WindowCreator::Create() const {
        if (!glfwInit()) {
            throw std::runtime_error("glfwInit failed!");
        }

        for (const auto [hint, value] : hints) {
            glfwWindowHint(hint, value);
        }
        
        GLFWwindow *window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if (window == nullptr) {
            glfwTerminate();
            throw std::runtime_error("glfwCreateWindow failed!");
        }
        
        return std::shared_ptr<Window>(new Window(window, width, height, title));
    }

    Window::Window(GLFWwindow *window, int width, int height, const std::string& title) 
        : handle(window),
        width(width),
        height(height),
        title(title),
        eventListener(InputsAndEventsManager::CreateEventListener())
    {
        std::function<void(int,int,int,int)> staticFuncKey = std::bind(&Window::keyCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);   
        std::function<void(int,int)> staticFuncFramebufferSize = std::bind(&Window::framebufferSizeCallback, this, std::placeholders::_1, std::placeholders::_2);   
        
        eventListener->SetKeyCallback(staticFuncKey);
        eventListener->SetWindowSizeCallback(staticFuncFramebufferSize);
    }

    Window::~Window() 
    {
        glfwDestroyWindow(handle);
    }

    void Window::RequestClose() { 
        glfwSetWindowShouldClose(handle, GLFW_TRUE);
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

    void Window::MakeCurrent()  {
        glfwMakeContextCurrent(handle);
    }
    
    void Window::SwapBuffers() {
        glfwSwapBuffers(handle);
    }

    void Window::framebufferSizeCallback(int width, int height) {
        this->width = width;
        this->height = height;
    }

    void Window::keyCallback(int key, int scancode, int action, int mods) {
        if (action != GLFW_PRESS)
            return;

        switch (key) {
            // case GLFW_KEY_ESCAPE:
            //     glfwSetWindowShouldClose(handle, GLFW_TRUE);
            //     break;
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

}