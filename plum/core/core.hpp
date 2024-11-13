#ifndef CORE_HPP
#define CORE_HPP

#include <glad/gl.h>
#include <plum/vertex.hpp>

namespace Core {

    class Vao : public GlObject {
        Vao();
        GLuint Generate() override;
        void Bind() override;
        void Unbind() override;
        void Setup(std::vector<Component::Vertex>& vertices);
    };

    class Vbo : public GlObject {
        Vbo();
        GLuint Generate();
        void Bind();
        void Unbind();
        void Setup(std::vector<Component::Vertex>& vertices);
    };

    class Ebo : public GlObject {
        Ebo();
        GLuint Generate();
        void Bind();
        void Unbind();
        void Setup(std::vector<Component::Index>& indices);
    };

    class Fbo : public GlObject {
        Fbo();
        GLuint Generate();
        void Bind();
        void Unbind();
    };

    class Rbo : public GlObject {
        Rbo();
        GLuint Generate();
        void Bind();
        void Unbind();
    };

    class GlObject {
        public:
            virtual GLuint Generate() = 0;
            virtual void Bind() = 0;
            virtual void Unbind() = 0;
            virtual ~GlObject() {}
        protected:
            GLuint handle;
            GLenum target;
    };

    class GlState {

    };

}

#endif