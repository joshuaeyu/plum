#pragma once

#include <plum/context/inputsevents.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <map>
#include <memory>
#include <string>

namespace Context {

    class Window {
        public:
            void MakeCurrent();
            void SwapBuffers();
            
            void SetTitle(const char* title);
            void SetWindowSize(int width, int height);
            void SetInputMode(int mode, int value);
            
            std::string Title() const { return title; }          
            float Aspect() const { return static_cast<float>(width)/height; }
            int Width() const { return width; }
            int Height() const { return height; }
            bool ShouldClose() const;
            GLFWwindow* Handle() const { return handle; }
        
        private:
            friend class WindowCreator;
            Window(GLFWwindow *window, int width, int height, const std::string& title);
            
            int width, height;
            std::string title;
            GLFWwindow* handle;

            void keyCallback(int key, int scancode, int action, int mods);
        
            std::shared_ptr<EventListener> eventListener;
        
        public:
            // Rule of five
            ~Window();
            Window(const Window& other) = delete;
            Window(Window&& other) = delete;
            Window& operator=(const Window& other) = delete;
            Window& operator=(Window&& other) = delete;
    };

    class WindowCreator {
        public:
            WindowCreator();

            std::shared_ptr<Window> Create() const;

            std::string title = "Plum Engine v2.0";
            int width = 1920;
            int height = 1080;
            std::map<int, int> hints;
    };
}