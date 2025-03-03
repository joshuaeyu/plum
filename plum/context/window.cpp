#include <plum/context/window.hpp>

#include <GLFW/glfw3.h>
// #include <imgui/imgui.h>
// #include <imgui/imgui_impl_glfw.h>
// #include <imgui/imgui_impl_opengl3.h>

#include <iostream>

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
    
    Window WindowCreator::Create() {
        if (!glfwInit())
            exit(-1);

        for (const auto [hint, value] : hints) {
            glfwWindowHint(hint, value);
        }
        
        GLFWwindow *window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if (window == nullptr) {
            std::cerr << "glfwCreateWindow failed" << std::endl;
            glfwTerminate();
            exit(-1);
        }

        return Window(window, width, height, title);
    }

    Window::Window(GLFWwindow *window, int width, int height, std::string title) 
        : handle(window),
        width(width),
        height(height),
        title(title),
        inputManager({GLFW_KEY_SPACE, GLFW_KEY_GRAVE_ACCENT})
    {}

    // void Window::SetFramebufferSizeCallback(GLFWframebuffersizefun callback) {
    //     glfwSetFramebufferSizeCallback(handle, callback);
    // }

    // void Window::SetCursorPosCallback(GLFWcursorposfun callback) {
    //     glfwSetCursorPosCallback(handle, callback);
    // }

    // void Window::SetKeyCallback(GLFWkeyfun callback) {
    //     glfwSetKeyCallback(handle, callback);
    // }

    void Window::SetInputMode(const int mode, const int value) {
        glfwSetInputMode(handle, mode, value);
    }

    bool Window::ShouldClose() const {
        return glfwWindowShouldClose(handle);
    }
    void Window::PollEvents() {
        glfwPollEvents();
        ProcessInputs();
    }
    void Window::SwapBuffers() {
        glfwSwapBuffers(handle);
    }

    void Window::MakeCurrent()  {
        glfwMakeContextCurrent(handle);
    }

    void Window::ProcessInputs() {
        if (inputManager.GetKeyDown(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(handle, GLFW_TRUE);
        if (inputManager.GetKeyDown(GLFW_KEY_GRAVE_ACCENT)) {
            int inputmode = glfwGetInputMode(handle, GLFW_CURSOR);
            if (inputmode == GLFW_CURSOR_NORMAL) {
                glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                firstMouse = true;
            } else if (inputmode == GLFW_CURSOR_DISABLED) {
                glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
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