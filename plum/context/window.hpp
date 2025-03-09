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
            void SetWindowSize(const int width, const int height);
            void SetInputMode(const int mode, const int value);
            
            int Width() const { return width; }  
            int Height() const { return height; }  
            std::string Title() const { return title; }          

            static float CurrentTime();
            static float DeltaTime();

            void MakeCurrent();
            void PollInputsAndEvents();
            void SwapBuffers();
            
            void ProcessInputs();

            bool ShouldClose() const;
            GLFWwindow* Handle() const { return handle; }
            
            Window Clone() { return Window(*this); }

        private:
            
            int width, height;
            std::string title;
            
            GLFWwindow* handle;
            inline static float lastTime = glfwGetTime();;
            inline static float currentTime = glfwGetTime();

            void keyCallback(int key, int scancode, int action, int mods);
        
            WindowInputsAndEventsManager inputsAndEvents;
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