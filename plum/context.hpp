#include <glad/gl.h>

namespace Core {

    class Context {
        private:
        public:
            Context();
            
            void enableDepth();
            void enableBlend();
            void enableStencil();
            void enableCull();
            void enableFramebufferSrgb();

            void disableDepth();
            void disableBlend();
            void disableStencil();
            void disableCull();
            void disableFramebufferSrgb();

            void clearColor();
            void clearDepth();
            void clearStencil();

            void setViewport();
            void useProgram();

        protected:
    };

}