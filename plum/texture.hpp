#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <vector>
#include <iostream>

#include <glad/glad.h>

#include <external/stb/stb_image.h>

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

std::string texTypeStrings[] = {
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

std::string TexTypeToString(Tex_Type type) {
    return texTypeStrings[type];
}

class Tex {

    public:
        
        string Name, NameTemp;
        vector<string> Paths;
        GLuint ID;
        GLenum Target;
        GLint Wrap;
        GLint Filter;
        Tex_Type Type;
        GLenum TexUnit;
        
        // Single filename (string)
        Tex(std::string name, std::string filename, GLenum target, GLint wrap, GLint filter = GL_LINEAR, Tex_Type type = TEX_DIFFUSE, bool flip = true) 
            : Name(name), NameTemp(name), Paths({filename}), Target(target), Wrap(wrap), Filter(filter), Type(type)
        {
            std::cout << "  Loading texture: " << filename << std::endl;
            if (target != GL_TEXTURE_2D) {
                cout << "Tex: Target must be GL_TEXTURE_2D" << endl;
                exit(-1);
            }
            init();
            load(filename, target, flip);
        }

        // Vector of filenames (vector<string>)
        Tex(std::string name, std::vector<std::string>& faces_filenames, GLenum target, GLint wrap, GLint filter = GL_LINEAR, bool flip = false)
            : Name(name), NameTemp(name), Paths(faces_filenames), Target(target), Wrap(wrap), Filter(filter), Type(TEX_DIFFUSE)
        {
            if (target != GL_TEXTURE_CUBE_MAP) {
                cout << "Tex: Target must be GL_TEXTURE_CUBE_MAP" << endl;
                exit(-1);
            }
            init();
            for (int i = 0; i < faces_filenames.size(); i++) {
                load(faces_filenames[i], GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, flip);
            }
        }

        ~Tex() {
            glDeleteTextures(1, &ID);
        }

    private:

        void load(std::string filename, GLenum target, bool flip) {
            // Load texture using stbi
            stbi_set_flip_vertically_on_load(flip);
            
            bool isHdr = stbi_is_hdr(filename.c_str());

            int width, height, num_channels;
            GLint internalformat;
            GLenum format;

            unsigned char *tex_data = nullptr;
            float *tex_dataf = nullptr;
            if (isHdr) {
                tex_dataf = stbi_loadf(filename.c_str(), &width, &height, &num_channels, 0);
                if (tex_dataf == nullptr) {
                    std::cerr << "stbi_load failed for " << filename << std::endl;
                    stbi_image_free(tex_data);
                    return;
                }
            }
            else {
                tex_data = stbi_load(filename.c_str(), &width, &height, &num_channels, 0);
                if (tex_data == nullptr) {
                    std::cerr << "stbi_load failed for " << filename << std::endl;
                    stbi_image_free(tex_data);
                    return;
                }
            }
            switch (num_channels) {
                case 1: 
                    format = GL_RED;
                    internalformat = GL_RED;
                    break;
                case 2:
                    format = GL_RG;
                    internalformat = GL_RG;
                    break;
                case 3: 
                    format = GL_RGB;
                    if (isHdr)
                        internalformat = GL_RGB32F;
                    else if (Type == TEX_DIFFUSE)
                        internalformat = GL_SRGB;   // correct any colors to linear space
                    else
                        internalformat = GL_RGB;
                    break;
                case 4: 
                    format = GL_RGBA;
                    if (isHdr)
                        internalformat = GL_RGBA32F;
                    else if (Type == TEX_DIFFUSE)
                        internalformat = GL_SRGB_ALPHA;   // correct any colors to linear space
                    else
                        internalformat = GL_RGBA;
                    break;
            }
            // Assign to target
            glActiveTexture(TexUnit);
            glBindTexture(Target, ID);
            if (isHdr) {
                glTexImage2D(target, 0, internalformat, width, height, 0, format, GL_FLOAT, tex_dataf);
            } else {
                glTexImage2D(target, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, tex_data);
            }
            glBindTexture(Target, 0);
            stbi_image_free(tex_data);
        }

    private: 

        void init() 
        {
            // Generate OpenGL texture
            glGenTextures(1, &ID);
            glActiveTexture(TexUnit);
            glBindTexture(Target, ID);
            if (Target == GL_TEXTURE_2D) {
                glTexParameteri(Target, GL_TEXTURE_WRAP_S, Wrap);
                glTexParameteri(Target, GL_TEXTURE_WRAP_T, Wrap);
                glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, Filter);
                glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, Filter);
            } else if (Target == GL_TEXTURE_CUBE_MAP) {
                glTexParameteri(Target, GL_TEXTURE_WRAP_S, Wrap);
                glTexParameteri(Target, GL_TEXTURE_WRAP_T, Wrap);
                glTexParameterf(Target, GL_TEXTURE_WRAP_R, Wrap);
                glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, Filter);
                glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, Filter);
            }
            glBindTexture(Target, 0);
        }
    
};

#endif