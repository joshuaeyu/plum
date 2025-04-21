#pragma once

#include <plum/asset/manager.hpp>
#include <glad/gl.h>

class ShaderAsset : public Asset {
    public:
        ShaderAsset(const Path& path, GLenum shader_type);
        ~ShaderAsset();

        GLuint Handle();

        void SyncWithFile() override;

    private:
        GLuint handle = -1;
        GLenum type;

        void setup();
};