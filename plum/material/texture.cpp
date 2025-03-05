#include <plum/material/texture.hpp>

#include <stb/stb_image.h>
#include <iostream>

namespace Material {


    Texture::Texture(std::string filename, bool flip, GLenum wrap, GLenum filter) 
    {
        loadFile(filename, flip, GL_TEXTURE_2D, wrap, filter);
    }
    
    Texture::Texture(std::vector<std::string> &cubemap_filenames, bool flip, GLenum wrap, GLenum filter) 
    {
        for (int i = 0; i < cubemap_filenames.size(); i++) {
            loadFile(cubemap_filenames[i], flip, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, wrap, filter);
        }
    }

    void Texture::loadFile(std::string filename, bool flip, GLenum target, GLenum wrap, GLenum filter) {
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
                else if (type == Diffuse)
                    internalformat = GL_SRGB;   // correct any colors to linear space
                else
                    internalformat = GL_RGB;
                break;
            case 4: 
                format = GL_RGBA;
                if (isHdr)
                    internalformat = GL_RGBA32F;
                else if (type == Diffuse)
                    internalformat = GL_SRGB_ALPHA;   // correct any colors to linear space
                else
                    internalformat = GL_RGBA;
                break;
        }
        // Assign to target
        if (isHdr) {
            tex = std::make_shared<Core::Tex2D>(target, internalformat, width, height, format, GL_FLOAT, wrap, filter);
            tex->DefineImage(tex_dataf);
        } else {
            tex = std::make_shared<Core::Tex2D>(target, internalformat, width, height, format, GL_UNSIGNED_BYTE, wrap, filter);
            tex->DefineImage(tex_data);
        }
        stbi_image_free(tex_dataf);
        stbi_image_free(tex_data);
    }


}