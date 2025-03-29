#pragma once

#include <plum/context/inputsevents.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <map>
#include <memory>
#include <string>

namespace Context {
    
    // GLFWglproc (*const GLLoadFunction)(const char *) = glfwGetProcAddress;

    class Window {
        public:
            Window(GLFWwindow *window, int width, int height, std::string title);
            ~Window();

            void SetTitle(const char* title);
            void SetWindowSize(int width, int height);
            void SetInputMode(int mode, int value);
            
            float Aspect() const { return static_cast<float>(width)/height; }
            int Width() const { return width; }
            int Height() const { return height; }
            std::string Title() const { return title; }          

            void MakeCurrent();
            void SwapBuffers();

            bool ShouldClose() const;
            GLFWwindow* Handle() const { return handle; }
            
            Window Clone() { return Window(*this); }

        private:
            int width, height;
            std::string title;
            
            GLFWwindow* handle;

            void keyCallback(int key, int scancode, int action, int mods);
        
            std::shared_ptr<WindowEventListener> eventListener;
    };

    class WindowCreator {
        public:
            WindowCreator();

            std::shared_ptr<Window> Create();

            std::string title = "Plum Engine";
            int width = 1920;
            int height = 1080;
            std::map<int, int> hints;
    };

    class Gui {
        public:
            Gui(GLFWwindow *window);
    };
}