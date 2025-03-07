#include <plum/context/inputsevents.hpp>

#include <plum/context/window.hpp>
#include <iostream>
namespace Context {

    // ========== WindowInputsAndEventsManager ==========

    void WindowInputsAndEventsManager::Setup(Window& window) {
        glfwSetCursorPosCallback(window.Handle(), cursor_pos_callback);
        glfwSetKeyCallback(window.Handle(), key_callback);
        glfwSetFramebufferSizeCallback(window.Handle(), framebuffer_size_callback);
        glfwSetWindowSizeCallback(window.Handle(), window_size_callback);
    }

    std::shared_ptr<WindowInputObserver> WindowInputsAndEventsManager::CreateInputObserver(std::vector<int> keysToMonitor) {
        std::shared_ptr<WindowInputObserver> wio = std::make_shared<WindowInputObserver>(keysToMonitor);
        observers.emplace_back(wio);
        return wio;
    }

    std::shared_ptr<WindowEventListener> WindowInputsAndEventsManager::CreateEventListener() {
        std::shared_ptr<WindowEventListener> wel = std::make_shared<WindowEventListener>();
        listeners.emplace_back(wel);
        return wel;
    }

    void WindowInputsAndEventsManager::cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {
        WindowInputObserver::setCursorPos(xpos, ypos);
        for (auto& weakListener : listeners) {
            if (auto listener = weakListener.lock())
                if (listener->cursorPosCallback)
                    listener->cursorPosCallback(xpos, ypos);
        }
    }
    
    void WindowInputsAndEventsManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        for (auto& weakObserver : observers) {
            if (auto observer = weakObserver.lock())
            observer->setKeyDown(key, action != GLFW_RELEASE);
        }
        for (auto& weakListener : listeners) {
            if (auto listener = weakListener.lock())
                if (listener->keyCallback)
                    listener->keyCallback(key, scancode, action, mods);
        }
    }
    
    void WindowInputsAndEventsManager::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        WindowInputObserver::setFramebufferSize(width, height);
        for (auto& weakListener : listeners) {
            if (auto listener = weakListener.lock())
                if (listener->framebufferSizeCallback)
                    listener->framebufferSizeCallback(width, height);
        }
    }

    void WindowInputsAndEventsManager::window_size_callback(GLFWwindow *window, int width, int height) {
        WindowInputObserver::setWindowSize(width, height);
        for (auto& weakListener : listeners) {
            if (auto listener = weakListener.lock())
                if (listener->windowSizeCallback)
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
    }
    WindowInputObserver::~WindowInputObserver() {
        // std::cout << "Destroying WindowInputObserver" << std::endl;
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
        WindowInputObserver::lastCursorX = WindowInputObserver::currCursorX;
        WindowInputObserver::lastCursorY = WindowInputObserver::currCursorY;
        WindowInputObserver::currCursorX = xpos;
        WindowInputObserver::currCursorY = ypos;
    }

    void WindowInputObserver::setFramebufferSize(int width, int height) {
        WindowInputObserver::framebufferWidth = width;
        WindowInputObserver::framebufferHeight = height;
    }

    void WindowInputObserver::setWindowSize(int width, int height) {
        WindowInputObserver::windowWidth = width;
        WindowInputObserver::windowHeight = height;
    }

    // ========== WindowEventListener ==========

    WindowEventListener::WindowEventListener() {}
    WindowEventListener::~WindowEventListener() {
        // std::cout << "Destroying WindowEventListener" << std::endl;
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