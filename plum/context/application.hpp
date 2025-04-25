#pragma once

#include <plum/context/window.hpp>

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
            
            static Application& Instance();

            void Run();

        private:
            Application();
            
            void predisplay();
            void display();
            void postdisplay();
    };

}