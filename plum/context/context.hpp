#pragma once

#include <plum/context/window.hpp>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <memory>

namespace Context {

    class Application {
        private:
            Application();
            void initialize();
            
        public:
            ~Application();
            static Application& Instance();
        
            std::shared_ptr<Window> defaultWindow;
            
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

        protected:
    };

}