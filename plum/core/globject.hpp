#pragma once

#include <plum/core/vertex.hpp>
#include <plum/core/tex.hpp>

#include <glad/gl.h>

#include <memory>

namespace Core {

    class GlObject {
        public:
            GLuint Handle() const;
            GLenum Target() const;
            virtual void Bind() = 0;
            virtual void Unbind() = 0;
        protected:
            GlObject() = default;
            GLuint handle;
            GLenum target;
        public:
            virtual ~GlObject() = default;
    };

    class Vbo : public GlObject {
        public:
            Vbo(const Vertex::VertexArray& varray);
            // Rule of five
            ~Vbo();
            Vbo(const Vbo& other) = delete;
            Vbo(Vbo&& other) = delete;
            Vbo& operator=(const Vbo& other) = delete;
            Vbo& operator=(Vbo&& other) = delete;

            Vertex::VertexArray vertexArray;
            
            void Bind() override;
            void Unbind() override;
    };

    class Ebo : public GlObject {
        public:
            Ebo(const std::vector<unsigned int>& indices);
            // Rule of five
            ~Ebo();
            Ebo(const Ebo& other) = delete;
            Ebo(Ebo&& other) = delete;
            Ebo& operator=(const Ebo& other) = delete;
            Ebo& operator=(Ebo&& other) = delete;

            std::vector<unsigned int> indices;
            
            void Bind() override;
            void Unbind() override;
    };

    class Vao : public GlObject {
        public:
            Vao(std::shared_ptr<Vbo> vb);
            Vao(std::shared_ptr<Vbo> vb, std::shared_ptr<Ebo> eb);
            // Rule of five
            ~Vao();
            Vao(const Vao& other) = delete;
            Vao(Vao&& other) = delete;
            Vao& operator=(const Vao& other) = delete;
            Vao& operator=(Vao&& other) = delete;

            void Bind() override;
            void Unbind() override;

            void Draw();

        private:
            std::shared_ptr<Vbo> vbo;
            std::shared_ptr<Ebo> ebo;            
            
            void setAttribPointerFormat();
    };

    class Ubo : public GlObject {
        public:
            Ubo(unsigned int index, size_t size);
            // Rule of five
            ~Ubo();
            Ubo(const Ubo& other) = delete;
            Ubo(Ubo&& other) = delete;
            Ubo& operator=(const Ubo& other) = delete;
            Ubo& operator=(Ubo&& other) = delete;

            void Bind() override;
            void Unbind() override;

            void UpdateData(unsigned int offset, size_t size, const void* data);
    };

    class Rbo : public GlObject {
        public:
            Rbo(int width, int height);
            // Rule of five
            ~Rbo();
            Rbo(const Rbo& other) = delete;
            Rbo(Rbo&& other) = delete;
            Rbo& operator=(const Rbo& other) = delete;
            Rbo& operator=(Rbo&& other) = delete;

            int width, height;
            
            void Bind() override;
            void Unbind() override;
            
            void Setup16();
            void Setup24();
            void Resize(int width, int height);
            
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
            
            int width, height;
            std::vector<std::shared_ptr<Tex>> colorAtts;
            std::shared_ptr<Tex> depthAtt;
            std::shared_ptr<Rbo> depthRboAtt;

            void Bind() override;
            void Unbind() override;
            void SetViewportDims();
            void ClearColor();
            void ClearColor(float r, float g, float b, float a);
            void ClearDepth();
            
            void AttachColorTex(std::shared_ptr<Tex> tex, int index = -1, int level = 0);
            void AttachColorTexCubeFace(int att_index, int face_idx, int level = 0);
            void AttachDepthRbo16();
            void AttachDepthRbo24();
            void AttachDepthTex(std::shared_ptr<Tex> tex, int level = 0);
            void UpdateDrawBuffers();
            void CheckStatus();
            
            void Resize(int width, int height);

            void BlitTo(Fbo& fbo, bool color = true, bool depth = true, int source_buffer_idx = 0, int target_buffer_idx = 0);
            void BlitToDefault(bool color = true, bool depth = true, int source_buffer_idx = 0);
            void BlitFrom(Fbo& fbo, bool color = true, bool depth = true, int source_buffer_idx = 0, int target_buffer_idx = 0);
    };

}