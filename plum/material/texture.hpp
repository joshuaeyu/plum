#pragma once

#include <plum/asset/image.hpp>
#include <plum/core/tex.hpp>

#include <memory>

namespace Material {
    
    enum TextureType {
        Diffuse,
        Specular,
        Ambient,
        Emissive,
        Height,
        Normal,
        Metalness,
        Roughness,
        Occlusion,
        Unknown
    };

    constexpr const char* texTypeStrings[10] = {
        "diffuse", 
        "specular", 
        "ambient", 
        "emissive",
        "height", 
        "normal", 
        "metalness", 
        "roughness", 
        "occlusion", 
        "unknown"
    };

    inline std::string TexTypeToString(TextureType type) {
        return texTypeStrings[type];
    }

    // textures should be thought of as image views?
    class Texture : public AssetUser {
        public:            
            Texture(std::shared_ptr<ImageAsset> image, TextureType type, GLenum wrap = GL_REPEAT, GLenum minfilter = GL_NEAREST);
            Texture(const std::vector<std::shared_ptr<ImageAsset>>& cubefaces, TextureType type, GLenum wrap = GL_REPEAT, GLenum minfilter = GL_NEAREST);
            
            std::string name;
            TextureType type;
            std::shared_ptr<Core::Tex2D> tex;
            std::vector<std::shared_ptr<ImageAsset>> images;

            void AssetResyncCallback() override;
        
        private:
            GLenum wrap;
            GLenum minfilter;

            void loadImage(std::shared_ptr<ImageAsset> image, GLenum target, int face_idx = -1);
    };

};