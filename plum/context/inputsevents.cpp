#include <plum/context/inputsevents.hpp>

namespace Context {

    // ========== WindowInputsAndEventsManager ==========

    void WindowInputsAndEventsManager::Setup(Window& window) {
        glfwSetCursorPosCallback(window.Handle(), cursor_pos_callback);
        glfwSetKeyCallback(window.Handle(), key_callback);
        glfwSetFramebufferSizeCallback(window.Handle(), framebuffer_size_callback);
        glfwSetWindowSizeCallback(window.Handle(), window_size_callback);
    }

    void WindowInputsAndEventsManager::cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {
        for (auto& observer : observers) {
            observer->setCursorPos(xpos, ypos);
        }
        for (auto& listener : listeners) {
            listener->cursorPosCallback(xpos, ypos);
        }
    }
    
    void WindowInputsAndEventsManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        for (auto& observer : observers) {
            observer->setKeyDown(key, action != GLFW_RELEASE);
        }
        for (auto& listener : listeners) {
            listener->keyCallback(key, scancode, action, mods);
        }
    }
    
    void WindowInputsAndEventsManager::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        for (auto& observer : observers) {
            observer->setFramebufferSize(width, height);
        }
        for (auto& listener : listeners) {
            listener->framebufferSizeCallback(width, height);
        }
    }

    void WindowInputsAndEventsManager::window_size_callback(GLFWwindow *window, int width, int height) {
        for (auto& observer : observers) {
            observer->setWindowSize(width, height);
        }
        for (auto& listener : listeners) {
            listener->windowSizeCallback(width, height);
        }
    }

    // ========== WindowInputObserver ==========

    WindowInputObserver::WindowInputObserver() {}
    WindowInputObserver::WindowInputObserver(std::vector<int> keysToMonitor)
    {
        for (int key : keysToMonitor) {
            keys[key] = false;
        }
        WindowInputsAndEventsManager::observers.emplace_back(this);
    }

    bool WindowInputObserver::GetKeyDown(int key) {
        bool result = false;
        std::map<int,bool>::iterator it = keys.find(key);
        if (it != keys.end()) {
            result = keys[key];
        }
        return result;
    }
                
    float WindowInputObserver::GetCursorX() { return currCursorX; }
    float WindowInputObserver::GetCursorY() { return currCursorY; }
    float WindowInputObserver::GetCursorDeltaX() { return currCursorX - lastCursorX; }
    float WindowInputObserver::GetCursorDeltaY() { return currCursorY - lastCursorY; }
    
    int WindowInputObserver::GetFramebufferWidth() { return framebufferWidth; }
    int WindowInputObserver::GetFramebufferHeight() { return framebufferHeight; }
    
    int WindowInputObserver::GetWindowWidth() { return windowWidth; }
    int WindowInputObserver::GetWindowHeight() { return windowHeight; }

    void WindowInputObserver::setKeyDown(int key, bool isDown) {
        std::map<int,bool>::iterator it = keys.find(key);
        if (it != keys.end()) {
            keys[key] = isDown;
        }
    }

    void WindowInputObserver::setCursorPos(double xpos, double ypos) {
        lastCursorX = currCursorX;
        lastCursorY = currCursorY;
        currCursorX = xpos;
        currCursorY = ypos;
    }

    void WindowInputObserver::setFramebufferSize(int width, int height) {
        framebufferWidth = width;
        framebufferHeight = height;
    }

    void WindowInputObserver::setWindowSize(int width, int height) {
        windowWidth = width;
        windowHeight = height;
    }

    // ========== WindowEventListener ==========

    WindowEventListener::WindowEventListener()
    {
        WindowInputsAndEventsManager::listeners.emplace_back(this);
    }

    void WindowEventListener::SetCursorPosCallback(std::function<void(double,double)> callback) {
        cursorPosCallback = callback;
    }
    void WindowEventListener::SetKeyCallback(std::function<void(int,int,int,int)> callback) {
        keyCallback = callback;
    }
    void WindowEventListener::SetFramebufferSizeCallback(std::function<void(int,int)> callback) {
        framebufferSizeCallback = callback;
    }
    void WindowEventListener::SetWindowSizeCallback(std::function<void(int,int)> callback) {
        windowSizeCallback = callback;
    }

}