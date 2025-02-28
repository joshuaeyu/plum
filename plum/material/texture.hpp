#pragma once

#include <plum/component/tex.hpp>
#include <memory>

namespace Material {

    class Texture {
        
        public:            
            enum TextureType {
                Ambient,
                Diffuse,
                Specular,
                Height,
                Normal, 
                Metalness,
                Roughness,
                Occlusion,
                Unknown
            };

            inline static const std::string texTypeStrings[9] = {
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

            static std::string TexTypeToString(TextureType type) {
                return texTypeStrings[type];
            }
            std::vector<std::string> paths;
            std::string name;
            TextureType type;

            
            Texture(std::string filename, bool flip = true, GLenum wrap = GL_REPEAT, GLenum filter = GL_NEAREST);
            Texture(std::vector<std::string>& cubemap_filenames, bool flip = true, GLenum wrap = GL_REPEAT, GLenum filter = GL_NEAREST);
            
        private:
            std::shared_ptr<Component::Tex> tex;

            void loadFile(std::string filename, bool flip, GLenum target, GLenum wrap, GLenum filter);
    };

};