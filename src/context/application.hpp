#pragma once

#include "context/window.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <memory>

class Demo;

namespace Context {

    class Application {
        public:
            Application(Application&) = delete;
            Application operator=(Application&) = delete;
            ~Application();

            std::shared_ptr<Window> activeWindow;
            std::shared_ptr<Demo> activeDemo;
            std::shared_ptr<Demo> requestedDemo;
            std::vector<std::shared_ptr<Demo>> demos;
            
            static Application& Instance();

            void Run();

        private:
            Application();
            
            void display();

            bool guiHeader();
            void guiFooter();
    };

}