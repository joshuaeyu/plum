#pragma once

#include <plum/context/inputsevents.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <map>
#include <memory>
#include <string>

namespace Context {
    
    GLFWglproc (*const GLLoadFunction)(const char *) = glfwGetProcAddress;

    class Window {
        public:
            Window(GLFWwindow *window, int width, int height, std::string title);

            void SetInputMode(const int mode, const int value);
            
            // bool GetKeyPress(int key) const;
            // bool GetKeyRelease(int key) const;

            static float CurrentTime();
            static float DeltaTime();

            void MakeCurrent();
            void PollEvents();
            void SwapBuffers();
            
            void ProcessInputs();

            bool ShouldClose() const;
            GLFWwindow* Handle() const { return handle; }
            
            Window Clone() { return Window(*this); }

            int width, height;
            std::string title;
            
        private:
            GLFWwindow* handle;
            inline static float lastTime = glfwGetTime();

            void keyCallback(int key, int scancode, int action, int mods);
        
            WindowEventListener eventListener;
    };

    class WindowCreator {
        public:
            WindowCreator();

            Window Create();

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