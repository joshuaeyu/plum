#include <GLFW/glfw3.h>
#include <plum/context/window.hpp>
#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "window.hpp"

namespace Context {

    WindowCreator::WindowCreator() {
        if (!glfwInit())
            exit(-1);
        settings[GLFW_CONTEXT_VERSION_MAJOR]    = 4;
        settings[GLFW_CONTEXT_VERSION_MINOR]    = 1;
        settings[GLFW_OPENGL_PROFILE]           = GLFW_OPENGL_CORE_PROFILE;
        settings[GLFW_OPENGL_FORWARD_COMPAT]    = GLFW_TRUE;
        settings[GLFW_SCALE_TO_MONITOR]         = GLFW_FALSE;
        settings[GLFW_COCOA_RETINA_FRAMEBUFFER] = GLFW_FALSE;
    }

    void WindowCreator::SetHint(const int hint, const int value) {
        settings[hint] = value;
    }

    void WindowCreator::SetHints(const int numhints, const int *hints, const int *values) {
        for (int i = 0; i < numhints; i++) {
            settings[hints[i]] = values[i];
        }
    }

    Window WindowCreator::Create(const int width, const int height, const char *title) {
        for (const auto [hint, value] : settings) {
            glfwWindowHint(hint, value);
        }

        GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (window == nullptr) {
            std::cerr << "glfwCreateWindow failed" << std::endl;
            glfwTerminate();
            exit(-1);
        }

        return Window(window);
    }

    Window::Window(GLFWwindow *window) {
        handle = window;
    }

    void Window::SetFramebufferSizeCallback(GLFWframebuffersizefun callback) {
        glfwSetFramebufferSizeCallback(handle, callback);
    }

    void Window::SetCursorPosCallback(GLFWcursorposfun callback) {
        glfwSetCursorPosCallback(handle, callback);
    }

    void Window::SetKeyCallback(GLFWkeyfun callback) {
        glfwSetKeyCallback(handle, callback);
    }

    void Window::SetInputMode(const int mode, const int value) {
        glfwSetInputMode(handle, mode, value);
    }

    bool Window::ShouldClose() const {
        return glfwWindowShouldClose(handle);
    }
    void Window::PollEvents() {
        glfwPollEvents();
    }
    void Window::SwapBuffers() {
        glfwSwapBuffers(handle);
    }

    void Window::MakeCurrent()
    {
        glfwMakeContextCurrent(handle);
    }

    // Gui::Gui(GLFWwindow *window) {
    //     IMGUI_CHECKVERSION();
    //     ImGui::CreateContext();
    //     ImGuiIO& io = ImGui::GetIO();
    //     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //     ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    //     ImGui_ImplOpenGL3_Init();
    // }

    // InputManager::KeyIsDown(const int key) {
    //     return keyDown[key];
    // }

}