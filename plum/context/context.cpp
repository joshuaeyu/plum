#include <plum/context/context.hpp>
#include <iostream>

namespace Context {

    GlContext& GlContext::GetGlContext() {
        static GlContext instance;
        return instance;
    }

    GlContext::GlContext() {}

    void GlContext::Initialize(Context::Window window) {
        if (!gladLoadGL(Context::GLLoadFunction)) {
            std::cerr << "gladLoadGLLoader failed" << std::endl;
            glfwTerminate();
            exit(-1);
        }

        EnableDepth();
        EnableCull();
        EnableFramebufferSrgb();
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // Doesn't change

        GLint n;
        glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &n); // 2048
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &n);  // 16
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &n); // 80
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &n); // 65536
        glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &n); // 16384
        glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &n); // 2048
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &n); // 16384
    }

    void GlContext::EnableDepth(GLenum func) {
        // Default LEQUAL for skybox optimization, since depth buffer is cleared to 1.0 by default
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(func);
    }
    void GlContext::EnableCull(GLenum mode) {
        glEnable(GL_CULL_FACE);
        glCullFace(mode);
    }
    void GlContext::EnableFramebufferSrgb() {
        glEnable(GL_FRAMEBUFFER_SRGB);
    }

    void GlContext::DisableDepth() {
        glDisable(GL_DEPTH_TEST);
    }
    void GlContext::DisableCull() {
        glDisable(GL_CULL_FACE);
    }
    void GlContext::DisableFramebufferSrgb() {
        glDisable(GL_FRAMEBUFFER_SRGB);
    }
    
    void ClearColor() {}
    void ClearDepth() {}
    void ClearStencil() {}
    
    void SetViewport() {}
    void UseProgram() {}
}