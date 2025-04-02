#pragma once

#include <GLFW/glfw3.h>

namespace Context {

    class Time {
        public:
            inline static void Update() {     
                lastTime = currentTime;
                currentTime = glfwGetTime();
                deltaTime = currentTime - lastTime;
            }
            inline static float CurrentTime() { return currentTime; }
            inline static float DeltaTime() { return deltaTime; }
        private:
            inline static float currentTime = 0.f;
            inline static float lastTime = 0.f;
            inline static float deltaTime = 0.f;
    };

}