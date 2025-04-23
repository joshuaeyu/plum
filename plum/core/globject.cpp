#include <plum/core/globject.hpp>
#include <plum/core/attribute.hpp>

#include <glad/gl.h>

#include <iostream>
#include <vector>

namespace Core {
    
    GLuint GlObject::Handle() const {
        return handle;
    }
    GLenum GlObject::Target() const {
        return target;
    }

    // Vertex array buffer
    Vbo::Vbo(const Vertex::VertexArray& varray) 
        : vertexArray(varray) 
    {
        glGenBuffers(1, &handle);
        Bind();
        // Create vertex data store
        glBufferData(GL_ARRAY_BUFFER, vertexArray.Size(), vertexArray.Data().data(), GL_STATIC_DRAW);
        Unbind();
    }
    Vbo::~Vbo() {
        // std::cout << "destroying Vbo" << std::endl;
        glDeleteBuffers(1, &handle);
    }
    void Vbo::Bind() {
        glBindBuffer(GL_ARRAY_BUFFER, handle);
    }
    void Vbo::Unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Element (index) array buffer
    Ebo::Ebo(const std::vector<unsigned int>& indices) 
        : indices(indices) 
    {
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
        // std::cout << "destroying Ebo" << std::endl;
        glDeleteBuffers(1, &handle);
    }

    // Vertex array
    Vao::Vao(std::shared_ptr<Vbo> vb) 
        : Vao(vb, nullptr) 
    {}
    Vao::Vao(std::shared_ptr<Vbo> vb, std::shared_ptr<Ebo> eb) 
        : vbo(std::move(vb)), 
        ebo(std::move(eb)) 
    {
        glGenVertexArrays(1, &handle);
        Bind();
        vbo->Bind();
        SetAttribPointerFormat();
        if (ebo)
            ebo->Bind();
        Unbind();
    }
    Vao::~Vao() {
        // std::cout << "destroying Vao" << std::endl;
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
        for (const auto& attr : Vertex::Attributes) {
            if (vbo->vertexArray.HasAttributes(attr.flag)) {
                glEnableVertexAttribArray(attr.index);
                glVertexAttribPointer(attr.index, attr.ncomps, GL_FLOAT, GL_FALSE, vbo->vertexArray.Stride(), (void *)(vbo->vertexArray.AttributeOffset(attr.flag)));
            }
        }
    }

    // Uniform buffer
    Ubo::Ubo(unsigned int index, size_t size) 
    {
        glGenBuffers(1, &handle);
        Bind();
        glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, index, handle);
        Unbind();
    }
    Ubo::~Ubo() {
        // std::cout << "destroying Ubo" << std::endl;
        glDeleteBuffers(1, &handle);
    }
    void Ubo::Bind() {
        glBindBuffer(GL_UNIFORM_BUFFER, handle);
    }
    void Ubo::Unbind() {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    void Ubo::UpdateData(unsigned int offset, size_t size, const void* data) {
        Bind();
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
        Unbind();
    }

    // Framebuffer
    Fbo::Fbo(int width, int height) 
        : width(width), 
        height(height) 
    {
        glGenFramebuffers(1, &handle);
    }
    Fbo::~Fbo() {
        // std::cout << "destroying Fbo" << std::endl;
        glDeleteFramebuffers(1, &handle);
    }
    void Fbo::Bind() {
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, handle);
    }
    void Fbo::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void Fbo::ClearColor() {
        ClearColor(0.f,0.f,0.f,1.f);
    }
    void Fbo::ClearColor(float r, float g, float b, float a) {
        glClearColor(r,g,b,a);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    void Fbo::ClearDepth() {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    void Fbo::AttachColorTex(std::shared_ptr<Tex> tex, int index, int level) {
        tex->Bind();
        
        if (index == -1) {
            index = colorAtts.size();
            colorAtts.resize(index + 1);
        } else if (index >= colorAtts.size()) {
            colorAtts.resize(index + 1);
        }
        
        switch (tex->target) {
            case GL_TEXTURE_2D:
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, tex->Handle(), level);
                break;
            case GL_TEXTURE_CUBE_MAP:
                // Must call AttachColorTexCubeFace to attach individual cubemap faces to this color attachment of the fbo!
                break;
        }
        
        colorAtts[index] = std::move(tex);
        UpdateDrawBuffers();
    }
    void Fbo::AttachColorTexCubeFace(int att_index, int face_idx, int level) {
        if (colorAtts[att_index]->target != GL_TEXTURE_CUBE_MAP) {
            std::cerr << "Fbo::AttachColorTexCubeFace error! Can only attach cubemap faces of GL_TEXTURE_CUBE_MAP textures!" << std::endl;
            exit(-1);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + att_index, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face_idx, colorAtts[att_index]->Handle(), level);
    }
    void Fbo::AttachDepthRbo16() {
        depthRboAtt = std::make_shared<Rbo>(width, height);
        depthRboAtt->Bind();
        depthRboAtt->Setup16();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRboAtt->Handle());
    }
    void Fbo::AttachDepthRbo24() {
        depthRboAtt = std::make_shared<Rbo>(width, height);
        depthRboAtt->Bind();
        depthRboAtt->Setup24();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRboAtt->Handle());
    }
    void Fbo::AttachDepthTex(std::shared_ptr<Tex> tex, int level) {
        depthAtt = std::move(tex);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthAtt->Handle(), level);
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
    void Fbo::Resize(int width, int height) {
        this->width = width;
        this->height = height;
        for (auto& color : colorAtts) {
            color->Resize(width, height);
        }
        if (depthAtt) {
            depthAtt->Resize(width, height);
        }
        if (depthRboAtt) {
            depthRboAtt->Bind();
            depthRboAtt->Resize(width, height);
        }
    }
    void Fbo::BlitTo(Fbo& target, bool color, bool depth, int source_buffer_idx, int target_buffer_idx) {
        GLbitfield mask = 0;
        mask |= GL_COLOR_BUFFER_BIT * color;
        mask |= GL_DEPTH_BUFFER_BIT * depth;
        
        glBindFramebuffer(GL_READ_FRAMEBUFFER, handle);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.handle);
        glReadBuffer(GL_COLOR_ATTACHMENT0 + source_buffer_idx);
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + target_buffer_idx);
        glBlitFramebuffer(0, 0, width, height, 0, 0, target.width, target.height, mask, GL_NEAREST);   // Internal formats need to match!
    }
    void Fbo::BlitToDefault(bool color, bool depth, int source_buffer_idx) {
        GLbitfield mask = 0;
        mask |= GL_COLOR_BUFFER_BIT * color;
        mask |= GL_DEPTH_BUFFER_BIT * depth;

        glBindFramebuffer(GL_READ_FRAMEBUFFER, handle);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glReadBuffer(GL_COLOR_ATTACHMENT0 + source_buffer_idx);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, mask, GL_NEAREST);   // Internal formats need to match!
    }
    void Fbo::BlitFrom(Fbo& source, bool color, bool depth, int source_buffer_idx, int target_buffer_idx) {
        source.BlitTo(*this, color, depth, source_buffer_idx, target_buffer_idx);
    }

    // Renderbuffer
    Rbo::Rbo(int width, int height) 
        : width(width), 
        height(height) 
    {
        glGenRenderbuffers(1, &handle);
    }
    Rbo::~Rbo() {
        // std::cout << "destroying Rbo" << std::endl;
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
        internalformat = GL_DEPTH_COMPONENT16;
        glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
    }
    void Rbo::Setup24() {
        internalformat = GL_DEPTH_COMPONENT24;
        glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
    }
    void Rbo::Resize(int width, int height) {
        this->width = width;
        this->height = height;
        if (internalformat == GL_DEPTH_COMPONENT16) {
            Setup16();
        } else if (internalformat == GL_DEPTH_COMPONENT24) {
            Setup24();
        }
    }

}