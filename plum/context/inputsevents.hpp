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
    // Assumes only a single window is used in the program since WindowInputsAndEventsManager is a singleton.

    class WindowInputObserver {

        public:
            WindowInputObserver();
            WindowInputObserver(std::vector<int> keysToMonitor);
            ~WindowInputObserver();

            bool GetKeyDown(int key);
            
            static float GetCursorX();
            static float GetCursorY();
            float GetCursorDeltaX();
            float GetCursorDeltaY();
            
            static int GetFramebufferWidth();
            static int GetFramebufferHeight();
            static int GetWindowWidth();
            static int GetWindowHeight();
            
        private:
            friend class WindowInputsAndEventsManager;
            
            static void perFrameRoutine();

            // For cursor
            static void setCursorPos(double xpos, double ypos);
            inline static float lastCursorX, lastCursorY, currCursorX, currCursorY;
            inline static bool firstCursor = true;
            inline static bool cursorMoved = false;
            // For key
            void setKeyDown(int key, bool isDown);
            std::map<int, bool> keys;
            // For framebuffer size
            static void setFramebufferSize(int width, int height);
            inline static int framebufferWidth, framebufferHeight;
            // For window size
            static void setWindowSize(int width, int height);
            inline static int windowWidth, windowHeight;
    };

    class WindowEventListener {
        public:
            WindowEventListener();
            ~WindowEventListener();

            void SetCursorPosCallback(std::function<void(double,double)> callback);
            void SetKeyCallback(std::function<void(int,int,int,int)> callback);
            void SetFramebufferSizeCallback(std::function<void(int,int)> callback);
            void SetWindowSizeCallback(std::function<void(int,int)> callback);

        private:
            friend class WindowInputsAndEventsManager;

            std::function<void(double,double)> cursorPosCallback;
            std::function<void(int,int,int,int)> keyCallback;
            std::function<void(int,int)> framebufferSizeCallback;
            std::function<void(int,int)> windowSizeCallback;
    };

    class WindowInputsAndEventsManager {
        public:
            // WindowInputsAndEvents(Window& window);
            static void Setup(Window& window);

            static std::shared_ptr<WindowInputObserver> CreateInputObserver(std::vector<int> keysToMonitor = {});
            static std::shared_ptr<WindowEventListener> CreateEventListener();

            static void PerFrameRoutine();

        private:
            friend class WindowInputObserver;
            friend class WindowEventListener;

            inline static std::vector<std::weak_ptr<WindowInputObserver>> observers;
            inline static std::vector<std::weak_ptr<WindowEventListener>> listeners;

            static void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos);
            static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
            static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
            static void window_size_callback(GLFWwindow *window, int width, int height);
    };
}