#pragma once

#include <plum/context/inputmanager.hpp>

#include <GLFW/glfw3.h>

#include <map>
#include <memory>

namespace Context {
    
    GLFWglproc (*const GLLoadFunction)(const char *) = glfwGetProcAddress;

    class WindowCreator {
        public:
            WindowCreator();

            Window Create();

            std::string title = "Plum Engine";
            int width = 1920;
            int height = 1080;
            std::map<int, int> hints;
    };

    class Window {
        public:
            Window(GLFWwindow *window, int width, int height, std::string title);
            // void AttachInputScheme(InputManager *scheme);
            // void SetFramebufferSizeCallback(GLFWframebuffersizefun callback);
            // void SetCursorPosCallback(GLFWcursorposfun callback);
            // void SetKeyCallback(GLFWkeyfun callback);
            void SetInputMode(const int mode, const int value);
            
            bool GetKeyPress(int key) const;
            bool GetKeyRelease(int key) const;

            static float CurrentTime();
            static float DeltaTime();

            void MakeCurrent();
            void PollEvents();
            void SwapBuffers();
            
            void ProcessInputs();

            bool ShouldClose() const;
            GLFWwindow* Handle() const { return handle; }
            
            Window Clone() { return Window(*this); }

            const int width, height;
            const std::string title;
            
        private:
            GLFWwindow* handle;
            static float lastTime, deltaTime;

            static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
            // Need some "manager" (event emitter) for framebuffer size
        
            InputManager inputManager;
    };

    class Gui {
        public:
            Gui(GLFWwindow *window);
    };
}