#pragma once

#include <plum/core/vertex.hpp>
#include <plum/core/tex.hpp>

#include <glad/gl.h>

#include <memory>

namespace Core {

    class GlObject {
        public:
            virtual void Bind() = 0;
            virtual void Unbind() = 0;
            // virtual ~GlObject() {}
            GLuint Handle() const;
            GLenum Target() const;
        protected:
            GLuint handle;
            GLenum target;
    };

    class Vbo : public GlObject {
        public:
            Vbo(const Vertex::VertexArray& varray);
            ~Vbo();
            void Bind();
            void Unbind();
            Vertex::VertexArray vertexArray;
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

    class Ubo : public GlObject {
        public:
            Ubo(unsigned int index, size_t size);
            ~Ubo();
            virtual void Bind();
            virtual void Unbind();
            void UpdateData(unsigned int offset, size_t size, const void* data);
    };

    class Rbo : public GlObject {
        public:
            Rbo(int width, int height);
            ~Rbo();
            void Bind();
            void Unbind();
            void Setup16();
            void Setup24();
            void Resize(int width, int height);
            int width, height;
        private:
            GLenum internalformat;
    };

    class Fbo : public GlObject {
        public:
            Fbo(int width, int height);
            // Rule of five
            ~Fbo();
            Fbo(const Fbo& other) = delete;
            Fbo(Fbo&& other) = delete;
            Fbo& operator=(const Fbo& other) = delete;
            Fbo& operator=(Fbo&& other) = delete;
            
            void Bind();
            void Unbind();
            
            void AttachColorTex(std::shared_ptr<Tex> tex, int index = -1, int level = 0);
            void AttachColorTexCubeFace(int att_index, int face_idx, int level = 0);
            void AttachDepthRbo16();
            void AttachDepthRbo24();
            void AttachDepthTex(std::shared_ptr<Tex> tex, int level = 0);
            void UpdateDrawBuffers();
            void CheckStatus();

            void Resize(int width, int height);
            
            void SetViewportDims();
            
            void ClearColor();
            void ClearColor(float r, float g, float b, float a);
            void ClearDepth();

            void BlitTo(Fbo& fbo, bool color = true, bool depth = true, int source_buffer_idx = 0, int target_buffer_index = 0);
            void BlitToDefault(bool color = true, bool depth = true, int source_buffer_index = 0);
            void BlitFrom(Fbo& fbo, bool color = true, bool depth = true, int source_buffer_idx = 0, int target_buffer_index = 0);

            int width, height;
            std::vector<std::shared_ptr<Tex>> colorAtts;
            std::shared_ptr<Tex> depthAtt;
            std::shared_ptr<Rbo> depthRboAtt;
    };


}