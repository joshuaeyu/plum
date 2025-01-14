#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <memory>
#include <GLFW/glfw3.h>

class Window {
    public:
        Window(GLFWwindow *window);
        void AttachInputScheme();
        
        GLFWwindow* glfwWindow;
    private:
};

class WindowManager {

    public:
        WindowManager();
        Window CreateWindow(int width, int height, const char *title);
        GLFWglproc (*loadFunction)(const char *) = glfwGetProcAddress;
    private:
        std::shared_ptr<Window> sdfs;
};

class InputScheme {

};

#endif