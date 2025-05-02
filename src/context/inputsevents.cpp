#include "context/inputsevents.hpp"

#include "context/window.hpp"
#include <iostream>

namespace Context {

    // ========== InputObserver ==========

    InputObserver::~InputObserver() {
        // std::clog << "Destroying InputObserver" << std::endl;
    }

    bool InputObserver::GetKeyDown(int key) {
        bool result = false;
        std::map<int,bool>::iterator it = keys.find(key);
        if (it != keys.end()) {
            result = keys[key];
        }
        return result;
    }

    float InputObserver::GetCursorDeltaX() {
        return cursorMoved ? currCursorX - lastCursorX : 0;
    }
    float InputObserver::GetCursorDeltaY() { 
        return cursorMoved ? currCursorY - lastCursorY : 0;
    }
    
    InputObserver::InputObserver(std::vector<int> keysToMonitor)
    {
        for (int key : keysToMonitor) {
            keys[key] = false;
        }
    }

    void InputObserver::prepareForFrameEvents() {
        cursorMoved = false;
    }

    void InputObserver::setKeyDown(int key, bool isDown) {
        std::map<int,bool>::iterator it = keys.find(key);
        if (it != keys.end()) {
            it->second = isDown;
        }
    }
    
    void InputObserver::setCursorEnabled(bool enabled) {
        cursorEnabled = enabled;
    }

    void InputObserver::setCursorPos(double xpos, double ypos) {
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

    void InputObserver::setFramebufferSize(int width, int height) {
        framebufferWidth = width;
        framebufferHeight = height;
    }

    void InputObserver::setWindowSize(int width, int height) {
        windowWidth = width;
        windowHeight = height;
    }

    // ========== EventListener ==========

    EventListener::~EventListener() {
        // std::clog << "Destroying EventListener" << std::endl;
    }

    void EventListener::SetCursorPosCallback(const std::function<void(double,double)>& callback) {
        cursorPosCallback = callback;
    }
    void EventListener::SetKeyCallback(const std::function<void(int,int,int,int)>& callback) {
        keyCallback = callback;
    }
    void EventListener::SetFramebufferSizeCallback(const std::function<void(int,int)>& callback) {
        framebufferSizeCallback = callback;
    }
    void EventListener::SetWindowSizeCallback(const std::function<void(int,int)>& callback) {
        windowSizeCallback = callback;
    }

    // ========== InputsAndEventsManager ==========

    void InputsAndEventsManager::Setup(Window* window) {
        InputsAndEventsManager::window = window;
        glfwSetCursorPosCallback(window->Handle(), cursor_pos_callback);
        glfwSetKeyCallback(window->Handle(), key_callback);
        glfwSetFramebufferSizeCallback(window->Handle(), framebuffer_size_callback);
        glfwSetWindowSizeCallback(window->Handle(), window_size_callback);
    }

    std::shared_ptr<InputObserver> InputsAndEventsManager::CreateInputObserver(std::vector<int> keysToMonitor) {
        auto observer = std::shared_ptr<InputObserver>(new InputObserver(keysToMonitor));
        observers.emplace_back(observer);
        return observer;
    }

    std::shared_ptr<EventListener> InputsAndEventsManager::CreateEventListener() {
        auto listener = std::shared_ptr<EventListener>(new EventListener);
        listeners.emplace_back(listener);
        return listener;
    }

    void InputsAndEventsManager::PollEvents() {
        for (auto& weakObserver : observers) {
            if (auto observer = weakObserver.lock())
                observer->prepareForFrameEvents();
        }
        glfwPollEvents();
        // need safety check for window ptr
        int cursorMode = glfwGetInputMode(window->Handle(), GLFW_CURSOR);
        for (auto& weakObserver : observers) {
            if (auto observer = weakObserver.lock())
                observer->setCursorEnabled(cursorMode != GLFW_CURSOR_NORMAL);
        }
    }

    void InputsAndEventsManager::cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {
        InputObserver::setCursorPos(xpos, ypos);
        for (auto& weakListener : listeners) {
            if (auto listener = weakListener.lock())
                if (listener->cursorPosCallback)
                    listener->cursorPosCallback(xpos, ypos);
        }
    }
    
    void InputsAndEventsManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
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
    
    void InputsAndEventsManager::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        InputObserver::setFramebufferSize(width, height);
        for (auto& weakListener : listeners) {
            if (auto listener = weakListener.lock())
                if (listener->framebufferSizeCallback)
                    listener->framebufferSizeCallback(width, height);
        }
    }

    void InputsAndEventsManager::window_size_callback(GLFWwindow *window, int width, int height) {
        InputObserver::setWindowSize(width, height);
        for (auto& weakListener : listeners) {
            if (auto listener = weakListener.lock())
                if (listener->windowSizeCallback)
                    listener->windowSizeCallback(width, height);
        }
    }

}