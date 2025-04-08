#pragma once

#include <plum/context/asset.hpp>
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

    class Texture : public Asset::Asset {
        
        public:            
            static std::string TexTypeToString(TextureType type) {
                return texTypeStrings[type];
            }
            std::string name;
            TextureType type;
            
            std::shared_ptr<Core::Tex2D> tex;

            Texture(Path path, TextureType type, bool flip = true, GLenum wrap = GL_REPEAT, GLenum minfilter = GL_NEAREST);
            Texture(const std::vector<Path>& cubemap_paths, TextureType type, bool flip = true, GLenum wrap = GL_REPEAT, GLenum minfilter = GL_NEAREST);

            // Asset::Asset
            void SyncWithDevice() override;
            
        private:
            bool flip;
            GLenum wrap;
            GLenum minfilter;

            void loadFile(std::string path, GLenum target, int face_idx = -1);
    };

};