#pragma once

#include <plum/asset/manager.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

class ModelAsset : public Asset {
    public:
        ModelAsset(const Path& path, float scale = 1.0f, bool flip_uvs = false);
        ~ModelAsset() = default;

        const aiScene* const Scene();

        void SyncWithFile() override;

    private:
        Assimp::Importer importer;  // Need to hold on to this to prevent scene destruction
        const aiScene* scene;
        float scale;
        bool flip;

        void import();
};