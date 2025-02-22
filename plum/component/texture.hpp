#pragma once

#include <string>
#include <vector>

#include <glad/gl.h>

static const std::string texTypeStrings[9] = {
    "ambient", 
    "diffuse", 
    "specular", 
    "height", 
    "normal", 
    "metalness", 
    "roughness", 
    "occlusion", 
    "unknown"
};

namespace Component {

    class Texture {

        public:
            
            enum Tex_Type {
                TEX_AMBIENT,
                TEX_DIFFUSE,
                TEX_SPECULAR,
                TEX_HEIGHT,
                TEX_NORMAL, 
                TEX_METALNESS,
                TEX_ROUGHNESS,
                TEX_OCCLUSION,
                TEX_UNKNOWN
            };

            static std::string TexTypeToString(Tex_Type type) {
                return texTypeStrings[type];
            }

            std::string Name, NameTemp;
            std::vector<std::string> Paths;
            GLuint ID;
            GLenum Target;
            GLint Wrap;
            GLint Filter;
            Tex_Type Type;
            GLenum TexUnit;
            
            // Single filename (string)
            Texture(std::string name, std::string filename, GLenum target, GLint wrap, GLint filter = GL_LINEAR, Tex_Type type = TEX_DIFFUSE, bool flip = true);

            // Vector of filenames (vector<string>)
            Texture(std::string name, std::vector<std::string>& faces_filenames, GLenum target, GLint wrap, GLint filter = GL_LINEAR, bool flip = false);

            // internal format, width, height, format, type
            static Texture Texture2D(GLenum target, GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint wrap, GLint filter);
            
            static Texture Texture3D(GLenum target, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint wrap, GLint filter);
            
            void Bind();
            void Unbind();

            ~Texture();

        private:

            void load(std::string filename, GLenum target, bool flip);

        private: 

            void init();
        
    };

}