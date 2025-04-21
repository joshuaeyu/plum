#include <plum/material/texture.hpp>

#include <stb/stb_image.h>
#include <iostream>

namespace Material {

    Texture::Texture(std::shared_ptr<ImageAsset> image, TextureType type, bool flip, GLenum wrap, GLenum minfilter)
        : images({image}),
        type(type),
        wrap(wrap),
        minfilter(minfilter)
    {
        loadImage(images[0], GL_TEXTURE_2D);
    }
    
    Texture::Texture(const std::vector<std::shared_ptr<ImageAsset>>& cubefaces, TextureType type, GLenum wrap, GLenum minfilter)
        : images(cubefaces),
        type(type),
        wrap(wrap),
        minfilter(minfilter)
    {
        for (int i = 0; i < cubefaces.size(); i++) {
            loadImage(images[i], GL_TEXTURE_CUBE_MAP, i);
        }
    }

    void Texture::loadImage(std::shared_ptr<ImageAsset> image, GLenum target, int face_idx) {
        GLint internalformat;
        GLenum format;

        switch (image->NumChannels()) {
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
                if (image->IsHdr())
                    internalformat = GL_RGB32F;
                else if (type == Diffuse || type == Emissive)
                    internalformat = GL_SRGB;   // correct any colors to linear space
                else
                    internalformat = GL_RGB;
                break;
            case 4: 
                format = GL_RGBA;
                if (image->IsHdr())
                    internalformat = GL_RGBA32F;
                else if (type == Diffuse || type == Emissive)
                    internalformat = GL_SRGB_ALPHA;   // correct any colors to linear space
                else
                    internalformat = GL_RGBA;
                break;
        }
        // Assign to target
        if (!tex) {
            if (image->IsHdr()) {
                tex = std::make_shared<Core::Tex2D>(target, internalformat, image->Width(), image->Height(), format, GL_FLOAT, wrap, minfilter, false, image->IsHdr());
            } else {
                tex = std::make_shared<Core::Tex2D>(target, internalformat, image->Width(), image->Height(), format, GL_UNSIGNED_BYTE, wrap, minfilter, false, image->IsHdr());
            }
        }
        if (face_idx <= -1) {
            tex->DefineImage(image->Data());
        } else {
            tex->DefineImageCubeFace(face_idx, image->Data());
        }
    }

    void Texture::AssetResyncCallback() {
        if (images.size() == 1) {
            loadImage(images[0], GL_TEXTURE_2D);
        } else {
            for (int i = 0; i < images.size(); i++) {
                loadImage(images[i], GL_TEXTURE_CUBE_MAP, i);
            }
        }
    }

    // std::vector<File> Texture::texturesToFiles(const std::vector<std::shared_ptr<Texture>> textures) {
    //     std::vector<File> files;
    //     for (const auto& texture : textures) {
    //         files.push_back(texture->GetFile());
    //     }
    //     return files;
    // }

}