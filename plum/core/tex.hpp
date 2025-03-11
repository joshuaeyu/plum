#pragma once

#include <glad/gl.h>

#include <string>
#include <vector>

namespace Core {

    class Tex {
        public:

            const GLenum target;
            const GLint internalformat;
            const GLsizei width, height, depth;
            const GLenum format, datatype;
            const GLint wrap, minfilter;

            virtual void DefineImage(const void* pixels, const int level = 0) {}

            GLuint Handle() const;
            void Bind();
            void Bind(int texunit);
            void Unbind();
            
            ~Tex();
            
        protected:
            
            Tex(GLenum target, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum datatype, GLint wrap, GLint minfilter, bool isshadowmap);
            
            virtual void initialize();
            
            GLuint handle;
            const bool isShadowmap;
            
            int textureUnit = -1;
            
    };
        
    class Tex2D : public Tex {
        public:
            Tex2D(GLenum target, GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint wrap, GLint minfilter, bool isshadowmap = false);
            
            void DefineImage(const void* pixels, const int level = 0) override;
            void DefineImageCubeFace(const int face_idx, const void* pixels, const int level = 0);
    };
    
    class Tex3D : public Tex {
        public:
            Tex3D(GLenum target, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint wrap, GLint minfilter, bool isshadowmap = false);

            void DefineImage(const void* pixels, const int level = 0) override;
    };

}