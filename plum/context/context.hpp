#pragma once

#include <glad/gl.h>
#include <plum/context/window.hpp>

namespace Context {

    class GlContext {
        private:
            GlContext();

        public:
            static GlContext& GetGlContext();
            void Initialize(Context::Window window);
            
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