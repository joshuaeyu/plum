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
        auto observer = std::shared_ptr<WindowInputObserver>(new WindowInputObserver(keysToMonitor));
        observers.emplace_back(observer);
        return observer;
    }

    std::shared_ptr<WindowEventListener> WindowInputsAndEventsManager::CreateEventListener() {
        auto listener = std::shared_ptr<WindowEventListener>(new WindowEventListener);
        listeners.emplace_back(listener);
        return listener;
    }

    void WindowInputsAndEventsManager::PerFrameRoutine() {
        for (auto& weakObserver : observers) {
            if (auto observer = weakObserver.lock())
                observer->perFrameRoutine();
        }
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
    float WindowInputObserver::GetCursorDeltaX() {
        return cursorMoved ? currCursorX - lastCursorX : 0;
    }
    float WindowInputObserver::GetCursorDeltaY() { 
        return cursorMoved ? currCursorY - lastCursorY : 0;
    }
    
    int WindowInputObserver::GetFramebufferWidth() { return framebufferWidth; }
    int WindowInputObserver::GetFramebufferHeight() { return framebufferHeight; }
    
    int WindowInputObserver::GetWindowWidth() { return windowWidth; }
    int WindowInputObserver::GetWindowHeight() { return windowHeight; }
    
    void WindowInputObserver::perFrameRoutine() {
        cursorMoved = false;
    }

    void WindowInputObserver::setKeyDown(int key, bool isDown) {
        std::map<int,bool>::iterator it = keys.find(key);
        if (it != keys.end()) {
            it->second = isDown;
        }
    }
    
    void WindowInputObserver::setCursorPos(double xpos, double ypos) {
        if (firstCursor) {
            lastCursorX = xpos;
            lastCursorY = ypos;
            firstCursor = false;
        } else {
            lastCursorX = currCursorX;
            lastCursorY = currCursorY;
        }
        currCursorX = xpos;
        currCursorY = ypos;
        cursorMoved = true;
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

    WindowEventListener::~WindowEventListener() {
        // std::cout << "Destroying WindowEventListener" << std::endl;
    }

    void WindowEventListener::SetCursorPosCallback(const std::function<void(double,double)>& callback) {
        cursorPosCallback = callback;
    }
    void WindowEventListener::SetKeyCallback(const std::function<void(int,int,int,int)>& callback) {
        keyCallback = callback;
    }
    void WindowEventListener::SetFramebufferSizeCallback(const std::function<void(int,int)>& callback) {
        framebufferSizeCallback = callback;
    }
    void WindowEventListener::SetWindowSizeCallback(const std::function<void(int,int)>& callback) {
        windowSizeCallback = callback;
    }

}