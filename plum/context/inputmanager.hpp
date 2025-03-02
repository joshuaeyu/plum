#pragma once

#include <vector>
#include <map>
#include <GLFW/glfw3.h>
#include <plum/context/window.hpp>

namespace Context {
    
    class InputManager {
        // Adapted from https://stackoverflow.com/questions/55573238/how-do-i-do-a-proper-input-class-in-glfw-for-a-game-engine
        
        // Assumes only a single window is used in the program

        public:
            // Camera class will have some InputScheme member object
            InputManager();
            InputManager(std::vector<int> keysToMonitor);

            static void Setup(Window& window);

            bool GetKeyDown(int key);
            static float GetMouseX();
            static float GetMouseY();
            static float GetMouseDeltaX();
            static float GetMouseDeltaY();
            
        private:
            bool SetKeyDown(int key, bool isDown);
            
            std::map<int, bool> keys;

            static float lastX, lastY, currX, currY;
            static bool firstMouse;

            static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
            static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
            // key press/release -> update inputscheme instances 
            // mouse movement -> update input sceheme instances 

    };

}