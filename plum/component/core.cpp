#include <glad/gl.h>
#include <plum/component/core.hpp>
#include <plum/vertex.hpp>
#include <plum/texture.hpp>

#include <vector>
#include <memory>

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

    // // Framebuffer
    // Fbo::Fbo(int width, int height) : width(width), height(height) {
    //     glGenFramebuffers(1, &handle);
    // }
    // Fbo::~Fbo() {
    //     glDeleteFramebuffers(1, &handle);
    // }
    // void Fbo::Bind() {
    //     glViewport(0, 0, width, height);
    //     glBindFramebuffer(GL_FRAMEBUFFER, handle);
    // }
    // void Fbo::Unbind() {
    //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // }
    // void Fbo::AttachDepth16() {
    //     depthAtt = std::make_shared<Rbo>(width, height);
    //     depthAtt->Setup16();
    //     glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthAtt->handle);
    // }
    // void Fbo::AttachDepth24() {
    //     depthAtt = std::make_shared<Rbo>(width, height);
    //     depthAtt->Setup24();
    //     glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthAtt->handle);
    // }
    // void Fbo::AttachColor2d(Tex& tex, int index, int level) {
    //     glBindTexture(GL_TEXTURE_2D, tex.ID);
    //     if (index > 0) {
    //         glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, tex.ID, level);
    //         colorAtts[index] = std::make_shared<Tex>(tex);
    //     } else {
    //         glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAtts.size(), GL_TEXTURE_2D, tex.ID, level);
    //         colorAtts.push_back(std::make_shared<Tex>(tex));
    //     }
    // }
    // void Fbo::AttachColor2dCube(Tex& tex, int index, int level) {
    //     glBindTexture(GL_TEXTURE_CUBE_MAP, tex.ID);
    //     if (index > 0) {
    //         for (int i = 0; i < 6; i++) {
    //             glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, tex.ID, level);
    //         }
    //         colorAtts[index] = std::make_shared<Tex>(tex);
    //     } else {
    //         for (int i = 0; i < 6; i++) {
    //             glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAtts.size(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, tex.ID, level);
    //         }
    //         colorAtts.push_back(std::make_shared<Tex>(tex));
    //     }
    // }

    
    // // Renderbuffer
    // Rbo::Rbo(int width, int height) : width(width), height(height) {
    //     glGenRenderbuffers(1, &handle);

    // }
    // Rbo::~Rbo() {
    //     glDeleteRenderbuffers(1, &handle);
    // }
    // void Rbo::Bind() {
    //     glViewport(0, 0, width, height);
    //     glBindRenderbuffer(GL_RENDERBUFFER, handle);
    // }
    // void Rbo::Unbind() {
    //     glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // }
    // void Rbo::Setup16() {
    //     Bind();
    //     glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    // }
    // void Rbo::Setup24() {
    //     Bind();
    //     glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    // }

}