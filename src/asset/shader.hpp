#pragma once

#include "asset/manager.hpp"
#include <glad/gl.h>

class ShaderAsset : public Asset {
    public:
        ShaderAsset(const Path& path, GLenum shader_type);
        ~ShaderAsset();

        GLenum Type() const { return type; }
        
        void SetType(GLenum shader_type) { type = shader_type; }
        
        GLuint Handle();

    private:
        GLuint handle = -1;
        GLenum type;

        void setup();
        void syncWithFile() override;
};