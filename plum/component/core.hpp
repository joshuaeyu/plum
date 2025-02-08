#pragma once

#include <glad/gl.h>
#include <plum/component/vertex.hpp>
#include <memory>

namespace Component {

    class GlObject {
        public:
            virtual void Bind() = 0;
            virtual void Unbind() = 0;
            virtual ~GlObject() {}
            GLuint Handle();
            GLenum Target();
        protected:
            GLuint handle;
            GLenum target;
    };


    class Vbo : public GlObject {
        public:
            Vbo(const Component::VertexArray& varray);
            ~Vbo();
            void Bind();
            void Unbind();
            Component::VertexArray vertexArray;
    };

    class Ebo : public GlObject {
        public:
            Ebo(const std::vector<unsigned int>& indices);
            ~Ebo();
            void Bind();
            void Unbind();
            std::vector<unsigned int> indices;
    };

    class Vao : public GlObject {
        public:
            Vao(std::shared_ptr<Vbo> vb);
            Vao(std::shared_ptr<Vbo> vb, std::shared_ptr<Ebo> eb);
            ~Vao();
            void Bind() override;
            void Unbind() override;
            void Draw();
        private:
            void SetAttribPointerFormat();
            std::shared_ptr<Vbo> vbo;
            std::shared_ptr<Ebo> ebo;            
    };
    // class Fbo : public GlObject {
    //     public:
    //         Fbo(int width, int height);
    //         ~Fbo();
    //         void Bind();
    //         void Unbind();
    //         void AttachDepth16();
    //         void AttachDepth24();
    //         void AttachColor2d(Tex& tex, int index = 0, int level = 0);
    //         void AttachColor2dCube(Tex& tex, int index = 0, int level = 0);
    //         void AttachTexture3d();
    //         int width, height;
    //         std::vector<Tex> colorAtts;
    //         std::shared_ptr<Rbo> depthAtt;
    // };

    // class Rbo : public GlObject {
    //     public:
    //         Rbo(int width, int height);
    //         ~Rbo();
    //         void Bind();
    //         void Unbind();
    //         void Setup16();
    //         void Setup24();
    //         friend Fbo; // Allow handle access
    //         int width, height;
    // };



}