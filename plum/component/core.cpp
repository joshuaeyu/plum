#include <glad/gl.h>
#include <plum/component/core.hpp>
#include <plum/vertex.hpp>
#include <plum/texture.hpp>

#include <vector>
#include <memory>
#include <iostream>

namespace Component {

    // Vertex array
    Vao::Vao(std::shared_ptr<Vbo> vb) : Vao(vb, nullptr) {}
    Vao::Vao(std::shared_ptr<Vbo> vb, std::shared_ptr<Ebo> eb) : vbo(vb), ebo(eb) {
        glGenVertexArrays(1, &handle);
        Bind();
        vbo->Bind();
        SetAttribPointerFormat();
        if (ebo)
            ebo->Bind();
        Unbind();
    }
    Vao::~Vao() {
        glDeleteVertexArrays(1, &handle);
    }
    void Vao::Bind() {
        glBindVertexArray(handle);
    }
    void Vao::Unbind() {
        glBindVertexArray(0);
    }
    void Vao::Draw() {
        Bind();
        if (ebo) {
            glDrawElements(GL_TRIANGLES, ebo->indices.size(), GL_UNSIGNED_INT, (void *)0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, vbo->vertexArray.VertexCount());
        }
    }
    void Vao::SetAttribPointerFormat() {
        // Specify vertex attribute pointer
        for (int i = 0; i < VertexAttrInfo.AttrList.size(); i++) {
            auto attr = VertexAttrInfo.AttrList[i];
            if (vbo->vertexArray.HasAttributes(attr.flag)) {
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i, attr.ncomps, GL_FLOAT, GL_FALSE, vbo->vertexArray.Stride(), (void *)(vbo->vertexArray.AttributeOffset(attr.flag)));
            }
        }
    }

    // Vertex array buffer
    Vbo::Vbo(const Component::VertexArray& varray) : vertexArray(varray) {
        glGenBuffers(1, &handle);
        Bind();
        // Create vertex data store
        glBufferData(GL_ARRAY_BUFFER, vertexArray.Size(), vertexArray.Data().data(), GL_STATIC_DRAW);
        Unbind();
    }
    Vbo::~Vbo() {
        glDeleteBuffers(1, &handle);
    }
    void Vbo::Bind() {
        glBindBuffer(GL_ARRAY_BUFFER, handle);
    }
    void Vbo::Unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Element (index) array buffer
    Ebo::Ebo(const std::vector<unsigned int>& indices) : indices(indices) {
        glGenBuffers(1, &handle);
        Bind();
        // Create index data store
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        Unbind();
    }
    void Ebo::Bind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
    }
    void Ebo::Unbind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    Ebo::~Ebo() {
        glDeleteBuffers(1, &handle);
    }

    // Uniform buffer
    Ubo::Ubo(const unsigned int index, const size_t size) {
        glGenBuffers(1, &handle);
        Bind();
        glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, index, handle);
        Unbind();
    }
    void Ubo::Bind() {
        glBindBuffer(GL_UNIFORM_BUFFER, handle);
    }
    void Ubo::Unbind() {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    void Ubo::UpdateData(const unsigned int offset, const size_t size, const void* data) {
        Bind();
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &handle);
        Unbind();
    }

    // Framebuffer
    Fbo::Fbo(int width, int height) : width(width), height(height) {
        glGenFramebuffers(1, &handle);
    }
    Fbo::~Fbo() {
        glDeleteFramebuffers(1, &handle);
    }
    void Fbo::Bind() {
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, handle);
    }
    void Fbo::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void ClearColor() {
        ClearColor(0,0,0,1);
    }
    void ClearColor(float r, float g, float b, float a) {
        glClearColor(r,g,b,a);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    void ClearDepth() {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    void Fbo::AttachColorTexture(Texture& texture, int index, int level) {
        texture.Bind();
        
        if (index == -1) {
            index = colorAtts.size();
            colorAtts.push_back(nullptr);
        }
        
        switch (texture.Target) {
            case GL_TEXTURE_2D:
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture.ID, level);
                break;
            case GL_TEXTURE_CUBE_MAP:
                for (int i = 0; i < 6; i++) {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texture.ID, level);
                }
                break;
        }
        
        colorAtts[index] = std::make_shared<Texture>(texture);
        UpdateDrawBuffers();
    }
    void Fbo::AttachDepthRbo16() {
        depthRboAtt = std::make_shared<Rbo>(width, height);
        depthRboAtt->Setup16();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRboAtt->handle);
    }
    void Fbo::AttachDepthRbo16() {
        depthRboAtt = std::make_shared<Rbo>(width, height);
        depthRboAtt->Setup24();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRboAtt->handle);
    }
    void Fbo::AttachDepthTexture(Texture& texture, int level = 0) {
        depthAtt = std::make_shared<Texture>(texture);
    }
    void Fbo::UpdateDrawBuffers() {
        std::vector<GLenum> buffers;
        for (int i = 0; i < colorAtts.size(); i++)
            buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
        glDrawBuffers(buffers.size(), buffers.data());
    }
    void Fbo::SetViewportDims() {
        glViewport(0, 0, width, height);
    }
    void Fbo::CheckStatus() {
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer error: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
        }
    }

    // Renderbuffer
    Rbo::Rbo(int width, int height) : width(width), height(height) {
        glGenRenderbuffers(1, &handle);
    }
    Rbo::~Rbo() {
        glDeleteRenderbuffers(1, &handle);
    }
    void Rbo::Bind() {
        glViewport(0, 0, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, handle);
    }
    void Rbo::Unbind() {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    void Rbo::Setup16() {
        Bind();
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    }
    void Rbo::Setup24() {
        Bind();
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    }

}