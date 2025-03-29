#pragma once

#include <plum/context/window.hpp>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <memory>

namespace Context {

    class Application {
        private:
            Application();
            
            // Enforce singleton
            Application(Application&) = delete;
            Application operator=(Application&) = delete;
            
            void initialize();

            float lastTime, currentTime;
            
        public:
            ~Application();

            static Application& Instance();
        
            std::shared_ptr<Window> defaultWindow;
            std::shared_ptr<Window> activeWindow;
            
            void PollInputsAndEvents();

            float CurrentTime();
            float DeltaTime();

            void EnableDepth(GLenum func = GL_LEQUAL);
            void EnableCull(GLenum mode = GL_BACK);
            void EnableFramebufferSrgb();
            // future: stencil, blend
        
            void DisableDepth();
            void DisableCull();
            void DisableFramebufferSrgb();

            void ClearColor();
            void ClearDepth();
            void ClearStencil();

            void SetViewport();
            void UseProgram();
    };

}