#include <GLFW/glfw3.h>
#include <plum/core/window.hpp>
#include <iostream>

WindowManager::WindowManager() {
    if (!glfwInit())
        exit(-1);
    // window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
}

Window WindowManager::CreateWindow(int width, int height, const char *title) {
    GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == nullptr) {
        std::cerr << "glfwCreateWindow failed" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    return Window(window);
}

Window::Window(GLFWwindow *window) {
    glfwWindow = window;
}