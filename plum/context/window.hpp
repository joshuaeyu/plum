#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <map>
#include <memory>
#include <GLFW/glfw3.h>

namespace Context {
    
    GLFWglproc (*const GLLoadFunction)(const char *) = glfwGetProcAddress;

    class InputManager {
        // Camera class will have some InputScheme member object

    };

    class Window {
        public:
            Window(GLFWwindow *window);
            // void AttachInputScheme(InputManager *scheme);
            void SetFramebufferSizeCallback(GLFWframebuffersizefun callback);
            void SetCursorPosCallback(GLFWcursorposfun callback);
            void SetKeyCallback(GLFWkeyfun callback);
            void SetInputMode(const int mode, const int value);
            
            void MakeCurrent();
            void SwapBuffers();
            
            bool ShouldClose() const;
            GLFWwindow* GetHandle() { return handle; }
            
            Window Clone() { return Window(*this); }
        private:
            GLFWwindow* handle;
    };

    class WindowCreator {

        public:
            WindowCreator();
            void SetHint(const int hint, const int value);
            void SetHints(const int numhints, const int *hints, const int *values);
            Window Create(int width, int height, const char *title);
        
        private:
            std::map<int, int> settings;
            
    };

    class Gui {
        public:
            Gui(GLFWwindow *window);
    };
}
#endif