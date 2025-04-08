#include <plum/context/asset.hpp>

#include <iostream>
#include <memory>

namespace Asset {

    Asset::Asset(const File& file) 
        : files({file})
    {}

    Asset::Asset(const std::vector<File>& files) 
        : files(files)
    {}

    Asset::Asset(const Path& path) 
        : files({path})
    {}

    Asset::Asset(const std::vector<Path>& paths) 
    {
        for (const auto& path : paths) {
            if (!path.IsEmpty()) {
                files.emplace_back(path);
            }
        }
    }

    bool Asset::NeedsResync() const {
        for (const auto& file : files) {
            if (file.NeedsResync()) {
                return true;
            }
        }
        return false;
    }

    void Asset::syncFilesWithDevice() {
        for (auto& file : files) {
            file.SyncWithDevice();
        }
    }

    AssetManager& AssetManager::Instance() {
        static AssetManager instance;
        return instance;
    }

    // void AssetManager::Initialize() {}

    void AssetManager::RemoveAsset(Path path) {
        RegistryHandle id;
        const auto it1 = coldAssets.find(path.RawPath());
        if (it1 != coldAssets.end()) {
            coldAssets.erase(it1);
            return;
        } 
        const auto it2 = hotAssets.find(path.RawPath());
        if (it2 != hotAssets.end()) {
            hotAssets.erase(it2);
            return;
        } 
        throw std::runtime_error("RemoveAsset: Asset not found!");
    }

    void AssetManager::ColdSyncWithDevice() {
        syncWithDevice(true);
    }

    void AssetManager::HotSyncWithDevice() {
        syncWithDevice(false);
    }

    void AssetManager::syncWithDevice(bool sync_cold) {
        for (const auto& [path, metadata] : hotAssets) {
            auto asset = assetRegistry.Get(metadata.id);
            if (asset->NeedsResync()) {
                asset->SyncWithDevice();
            }
        }
        if (sync_cold) {
            for (const auto& [path, metadata] : coldAssets) {
                auto asset = assetRegistry.Get(metadata.id);
                if (asset->NeedsResync()) {
                    asset->SyncWithDevice();
                }
            }
        }
    }

}