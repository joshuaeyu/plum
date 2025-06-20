#include "core/tex.hpp"

#include <string>
#include <glad/gl.h>

#include <stb/stb_image.h>

#include <iostream>

namespace Core {

    Tex::Tex(GLenum target, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum datatype, GLint wrap, GLint minfilter, bool is_shadowmap, bool is_hdr) 
        : target(target), 
        internalformat(internalformat), 
        width(width), 
        height(height), 
        depth(depth), 
        format(format), 
        datatype(datatype), 
        wrap(wrap), 
        minfilter(minfilter), 
        isShadowmap(is_shadowmap),
        isHdr(is_hdr)
    {
        initialize();
    }
    Tex::~Tex()
    {
        glDeleteTextures(1, &handle);
    }

    void Tex::Bind() {
        glBindTexture(target, handle);
    }
    void Tex::Bind(int texunit) {
        textureUnit = texunit;
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(target, handle);
    }
    void Tex::Unbind() {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(target, 0);
    }
    
    void Tex::GenerateMipMap() {
        glGenerateMipmap(target);
    }

    void Tex::initialize() {
        glGenTextures(1, &handle);
        Bind();
        // Determine an appopriate magnification filter
        GLint magfilter;
        switch (minfilter) {
            case GL_NEAREST:
            case GL_NEAREST_MIPMAP_NEAREST:
            case GL_LINEAR_MIPMAP_NEAREST:
                magfilter = GL_NEAREST;
                break;
            case GL_LINEAR:
            case GL_NEAREST_MIPMAP_LINEAR:
            case GL_LINEAR_MIPMAP_LINEAR:
                magfilter = GL_LINEAR;
        }
        // Set parameters
        switch (target) {
            case GL_TEXTURE_2D:
            case GL_TEXTURE_2D_ARRAY:
                glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
                glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minfilter);
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magfilter);
                if (isShadowmap) {
                    float border_color[] = {1.f, 1.f, 1.f, 1.f};
                    glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, border_color);
                    glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                }
                break;
            case GL_TEXTURE_CUBE_MAP:
            case GL_TEXTURE_CUBE_MAP_ARRAY:
                glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
                glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
                glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap);
                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minfilter);
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magfilter);
                if (isShadowmap) {
                    glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                }
                break;
            default:
                // throw error
                break;
        }
    }

    Tex2D::Tex2D(GLenum target, GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum datatype, GLint wrap, GLint minfilter, bool is_shadowmap, bool is_hdr)
    : Tex{target, internalformat, width, height, 0, format, datatype, wrap, minfilter, is_shadowmap, is_hdr}
    {
        Bind();
        switch (target) {
            case GL_TEXTURE_2D:
                DefineImage(nullptr);
                break;
            case GL_TEXTURE_CUBE_MAP:
                for (int i = 0; i < 6; i++)
                    DefineImageCubeFace(i, nullptr);
                break;
        }
    }

    void Tex2D::DefineImage(const void *pixels, int level) {
        Bind();
        glTexImage2D(target, level, internalformat, width, height, 0, format, datatype, pixels);
    }
    void Tex2D::DefineImageCubeFace(int face_idx, const void *pixels, int level) {
        Bind();
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face_idx, level, internalformat, width, height, 0, format, datatype, pixels);
    }

    void Tex2D::Resize(int width, int height) {
        this->width = width;
        this->height = height;
        switch (target) {
            case GL_TEXTURE_2D:
                DefineImage(nullptr);
                break;
            case GL_TEXTURE_CUBE_MAP:
                for (int i = 0; i < 6; i++)
                    DefineImageCubeFace(i, nullptr);
                break;
        }
    } 

    Tex3D::Tex3D(GLenum target, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum datatype, GLint wrap, GLint minfilter, bool is_shadowmap, bool is_hdr) 
    : Tex{target, internalformat, width, height, depth, format, datatype, wrap, minfilter, is_shadowmap, is_hdr}
    {
        Bind();
        DefineImage(nullptr);
    }

    void Tex3D::DefineImage(const void *pixels, int level) {
        Bind();
        glTexImage3D(target, level, internalformat, width, height, depth, 0, format, datatype, pixels);
    }

    void Tex3D::Resize(int width, int height) {
        this->width = width;
        this->height = height;
        DefineImage(nullptr);
    } 

}