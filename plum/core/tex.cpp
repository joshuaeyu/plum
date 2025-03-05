#include <plum/core/tex.hpp>

#include <string>
#include <glad/gl.h>

#include <stb/stb_image.h>

namespace Core {

    Tex::Tex(GLenum target, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum datatype, GLint wrap, GLint filter, bool isshadowmap) 
        : target(target), 
        internalformat(internalformat), 
        width(width), 
        height(height), 
        depth(depth), 
        format(format), 
        datatype(datatype), 
        wrap(wrap), 
        filter(filter), 
        isShadowmap(isshadowmap)
    {
        initialize();
    }
    Tex::~Tex()
    {
        glDeleteTextures(1, &handle);
    }

    void Tex::initialize() {
        glGenTextures(1, &handle);
        glBindTexture(target, handle);
        switch (target) {
            case GL_TEXTURE_2D:
            case GL_TEXTURE_2D_ARRAY:
                glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
                glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
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
                glTexParameterf(target, GL_TEXTURE_WRAP_R, wrap);
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
                if (isShadowmap) {
                    glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                }
                break;
            default:
                // throw error
                break;
        }
    }
    
    GLuint Tex::Handle() const {
        return handle;
    }
    void Tex::Bind() {
        glBindTexture(target, handle);
    }
    void Tex::Bind(int texunit) {
        textureUnit = texunit;
        glActiveTexture(GL_TEXTURE0 + texunit);
        glBindTexture(target, handle);
    }
    void Tex::Unbind() {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(target, 0);
    }

    Tex2D::Tex2D(GLenum target, GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum datatype, GLint wrap, GLint filter, bool isshadowmap)
    : Tex{target, internalformat, width, height, 0, format, datatype, wrap, filter, isshadowmap}
    {}

    void Tex2D::DefineImage(const void *pixels, const int level) {
        glTexImage2D(target, level, internalformat, width, height, 0, format, datatype, pixels);
    }

    Tex3D::Tex3D(GLenum target, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum datatype, GLint wrap, GLint filter, bool isshadowmap) 
    : Tex{target, internalformat, width, height, depth, format, datatype, wrap, filter, isshadowmap}
    {}

    void Tex3D::DefineImage(const void *pixels, const int level) {
        glTexImage3D(target, level, internalformat, width, height, depth, 0, format, datatype, pixels);
    }

}