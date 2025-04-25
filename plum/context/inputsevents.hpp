#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <functional>
#include <map>
#include <vector>

namespace Context {
    
    class Window;

    // Window inputs and events system.
    // Allows classes to actively observe inputs or passively listen for events, without having a direct reference to the main Window.
    // Adapted from https://stackoverflow.com/questions/55573238/how-do-i-do-a-proper-input-class-in-glfw-for-a-game-engine.
    // Assumes only a single window is used in the program since InputsAndEventsManager is a singleton.

    class InputObserver {
        public:
            ~InputObserver();

            bool GetKeyDown(int key);
            
            static bool GetCursorEnabled() { return cursorEnabled; }

            static float GetCursorX() { return currCursorX; }
            static float GetCursorY() { return currCursorY; }
            static float GetCursorDeltaX();
            static float GetCursorDeltaY();
            
            static int GetFramebufferWidth() { return framebufferWidth; }
            static int GetFramebufferHeight() { return framebufferHeight; }
            static int GetWindowWidth() { return windowWidth; }
            static int GetWindowHeight() { return windowHeight; }
            
        private:
            friend class InputsAndEventsManager;

            InputObserver() = default;
            InputObserver(std::vector<int> keysToMonitor);
            
            static void prepareForFrameEvents();

            // Cursor
            inline static bool cursorEnabled = false;
            inline static float lastCursorX, lastCursorY, currCursorX, currCursorY;
            inline static bool firstCursor = true;
            inline static bool cursorMoved = false;
            static void setCursorEnabled(bool enabled);
            static void setCursorPos(double xpos, double ypos);
            // Key
            std::map<int, bool> keys;
            void setKeyDown(int key, bool isDown);
            // Framebuffer size
            inline static int framebufferWidth, framebufferHeight;
            static void setFramebufferSize(int width, int height);
            // Window size
            inline static int windowWidth, windowHeight;
            static void setWindowSize(int width, int height);
    };

    class EventListener {
        public:
            ~EventListener();
            
            void SetCursorPosCallback(const std::function<void(double,double)>& callback);
            void SetKeyCallback(const std::function<void(int,int,int,int)>& callback);
            void SetFramebufferSizeCallback(const std::function<void(int,int)>& callback);
            void SetWindowSizeCallback(const std::function<void(int,int)>& callback);
        
        private:
            friend class InputsAndEventsManager;
            
            EventListener() = default;

            std::function<void(double,double)> cursorPosCallback;
            std::function<void(int,int,int,int)> keyCallback;
            std::function<void(int,int)> framebufferSizeCallback;
            std::function<void(int,int)> windowSizeCallback;
    };

    class InputsAndEventsManager {
        public:
            static void Setup(Window* window);

            static std::shared_ptr<InputObserver> CreateInputObserver(std::vector<int> keysToMonitor = {});
            static std::shared_ptr<EventListener> CreateEventListener();

            static void PollEvents();

        private:
            friend class InputObserver;
            friend class EventListener;

            inline static Window* window;
            inline static std::vector<std::weak_ptr<InputObserver>> observers;
            inline static std::vector<std::weak_ptr<EventListener>> listeners;

            static void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos);
            static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
            static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
            static void window_size_callback(GLFWwindow *window, int width, int height);
    };
}