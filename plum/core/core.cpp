#include <glad/gl.h>
#include <plum/core/core.hpp>
#include <plum/vertex.hpp>

    class Vao : public GlObject {
        Vao() {}
        GLuint Generate() {
            glGenVertexArrays(1, &handle);
            return handle;
        }
        void Bind() {
            glBindVertexArray(handle);
        }
        void Unbind();
    };

    class Vbo : public GlObject {
        Vbo() {
            target = GL_ARRAY_BUFFER;
        }
        GLuint Generate() {
            glGenBuffers(1, &handle);
            return handle;
        }
        void Bind() {
            glBindVertexArray(handle);
        }
    };

    class Ebo : public GlObject {
        Ebo() {
            target = GL_ELEMENT_ARRAY_BUFFER;
        }
        GLuint Generate() {
            glGenBuffers(1, &handle);
            return handle;
        }
        void Bind() {
            glBindVertexArray(handle);
        }
    };