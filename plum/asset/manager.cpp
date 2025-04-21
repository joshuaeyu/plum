#include <plum/asset/manager.hpp>

#include <iostream>
#include <memory>


AssetManager& AssetManager::Instance() {
    static AssetManager instance;
    return instance;
}

// void AssetManager::Initialize() {}

void AssetManager::Remove(const Path& path) {
    const auto it = assets.find(path.RawPath());
    if (it == assets.end()) {
        throw std::runtime_error("Remove: Asset not found!");
    } 
    assets.erase(it);
}

void AssetManager::ColdSyncWithDevice() {
    syncWithDevice(true);
}

void AssetManager::HotSyncWithDevice() {
    syncWithDevice(false);
}

void AssetManager::syncWithDevice(bool sync_cold) {
    for (const auto& [_, info] : assets) {
        if (sync_cold || info.hotReload) {
            if (info.asset->NeedsResync()) {
                info.asset->SyncWithFile();
            }
        }
    }
}