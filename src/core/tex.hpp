#pragma once

#include <glad/gl.h>

#include <string>
#include <vector>

namespace Core {

    class Tex {
        public:        
            // Rule of five
            virtual ~Tex();
            Tex(const Tex& other) = delete;
            Tex(Tex&& other) = delete;
            Tex& operator=(const Tex& other) = delete;
            Tex& operator=(Tex&& other) = delete;

            const GLenum target;
            const GLint internalformat;
            GLsizei width, height, depth;
            const GLenum format, datatype;
            const GLint wrap, minfilter;
            const bool isShadowmap;
            const bool isHdr;

            GLuint Handle() const { return handle; }
            
            void Bind();
            void Bind(int texunit);
            void Unbind();

            void GenerateMipMap();
            virtual void DefineImage(const void* pixels, int level = 0) = 0;
            virtual void Resize(int width, int height) = 0;
            
        protected:
            Tex(GLenum target, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum datatype, GLint wrap, GLint minfilter, bool is_shadowmap, bool is_hdr);
                        
            GLuint handle;
            int textureUnit = -1;

            virtual void initialize();
    };
        
    class Tex2D : public Tex {
        public:
            Tex2D(GLenum target, GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint wrap, GLint minfilter, bool is_shadowmap = false, bool is_hdr = false);
            
            void DefineImage(const void* pixels, int level = 0) override;
            void DefineImageCubeFace(int face_idx, const void* pixels, int level = 0);
            void Resize(int width, int height) override;
    };
    
    class Tex3D : public Tex {
        public:
            Tex3D(GLenum target, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint wrap, GLint minfilter, bool is_shadowmap = false, bool is_hdr = false);

            void DefineImage(const void* pixels, int level = 0) override;
            void Resize(int width, int height) override;
    };

}