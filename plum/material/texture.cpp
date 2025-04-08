#include <plum/material/texture.hpp>

#include <stb/stb_image.h>
#include <iostream>

namespace Material {

    Texture::Texture(Path path, TextureType type, bool flip, GLenum wrap, GLenum minfilter)
        : Asset::Asset(path),
        type(type),
        flip(flip),
        wrap(wrap),
        minfilter(minfilter)
    {
        loadFile(files[0].RawPath(), GL_TEXTURE_2D);
    }
    
    Texture::Texture(const std::vector<Path>& cubemap_paths, TextureType type, bool flip, GLenum wrap, GLenum minfilter)
        : Asset::Asset(cubemap_paths),
        type(type),
        flip(flip),
        wrap(wrap),
        minfilter(minfilter)
    {
        for (int i = 0; i < files.size(); i++) {
            loadFile(files[i].RawPath(), GL_TEXTURE_CUBE_MAP, i);
        }
    }

    void Texture::loadFile(std::string path, GLenum target, int face_idx) {
        // Load texture using stbi
        stbi_set_flip_vertically_on_load(flip);
            
        bool isHdr = stbi_is_hdr(path.c_str());

        int width, height, num_channels;
        GLint internalformat;
        GLenum format;

        unsigned char *tex_data = nullptr;
        float *tex_dataf = nullptr;
        if (isHdr) {
            tex_dataf = stbi_loadf(path.c_str(), &width, &height, &num_channels, 0);
            if (tex_dataf == nullptr) {
                std::cerr << "stbi_load failed for " << path << std::endl;
                stbi_image_free(tex_dataf);
                return;
            }
        } else {
            tex_data = stbi_load(path.c_str(), &width, &height, &num_channels, 0);
            if (tex_data == nullptr) {
                std::cerr << "stbi_load failed for " << path << std::endl;
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
                else if (type == Diffuse || type == Emissive)
                    internalformat = GL_SRGB;   // correct any colors to linear space
                else
                    internalformat = GL_RGB;
                break;
            case 4: 
                format = GL_RGBA;
                if (isHdr)
                    internalformat = GL_RGBA32F;
                else if (type == Diffuse || type == Emissive)
                    internalformat = GL_SRGB_ALPHA;   // correct any colors to linear space
                else
                    internalformat = GL_RGBA;
                break;
        }
        // Assign to target
        if (isHdr) {
            if (!tex) {
                tex = std::make_shared<Core::Tex2D>(target, internalformat, width, height, format, GL_FLOAT, wrap, minfilter, false, isHdr);
            }
            if (face_idx <= -1) {
                tex->DefineImage(tex_dataf);
            } else {
                tex->DefineImageCubeFace(face_idx, tex_dataf);
            }
            stbi_image_free(tex_dataf);
        } else {
            if (!tex) {
                tex = std::make_shared<Core::Tex2D>(target, internalformat, width, height, format, GL_UNSIGNED_BYTE, wrap, minfilter, false, isHdr);
            }
            if (face_idx <= -1) {
                tex->DefineImage(tex_data);
            } else {
                tex->DefineImageCubeFace(face_idx, tex_data);
            }
            stbi_image_free(tex_data);
        }
    }

    void Texture::SyncWithDevice() {
        if (files.size() == 1) {
            loadFile(files[0].RawPath(), GL_TEXTURE_2D);
        } else {
            for (int i = 0; i < files.size(); i++) {
                loadFile(files[i].RawPath(), GL_TEXTURE_CUBE_MAP, i);
            }
        }
    }

}