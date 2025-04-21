#include <plum/asset/model.hpp>

#include <assimp/postprocess.h>

#include <stdexcept>
#include <string>

ModelAsset::ModelAsset(const Path& path, float scale, bool flip_uvs) 
    : Asset(path),
    scale(scale),
    flip(flip_uvs)
{}

const aiScene* const ModelAsset::Scene() {
    if (!scene) {
        import();
    }
    return scene;
}

void ModelAsset::SyncWithFile() {
    import();
}

void ModelAsset::import() {
    unsigned int importerFlags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GlobalScale;
    if (flip)
        importerFlags |= aiProcess_FlipUVs;

    importer.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, scale);

    scene = importer.ReadFile(file.RawPath(), importerFlags);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::string errorMsg = "Error importing model! ";
        errorMsg += importer.GetErrorString();
        throw std::runtime_error(errorMsg);
    }
}
