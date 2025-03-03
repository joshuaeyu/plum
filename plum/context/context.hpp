#pragma once

#include <plum/context/window.hpp>

#include <glad/gl.h>

namespace Context {

    class GlContext {
        private:
            GlContext();

        public:
            static GlContext& GetGlContext();
            void Initialize();
            
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