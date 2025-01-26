#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <glad/gl.h>
#include <plum/context/window.hpp>

namespace Context {

    class GlContext {
        private:
            GlContext();

        public:
            static GlContext& GetGlContext();
            void Initialize(Context::Window window);
            
            void EnableDepth();
            void EnableBlend();
            void EnableStencil();
            void EnableCull();
            void EnableFramebufferSrgb();
        
            void DisableDepth();
            void DisableBlend();
            void DisableStencil();
            void DisableCull();
            void DisableFramebufferSrgb();

            void SetDepthFunc(GLenum func);
            void SetBlendFunc(GLenum sfactor, GLenum dfactor);
            void SetCullFace(GLenum mode);

            void ClearColor();
            void ClearDepth();
            
            void ClearStencil();

            void SetViewport();
            void UseProgram();

        protected:
    };

}

#endif