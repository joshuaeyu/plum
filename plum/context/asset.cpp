#include <plum/context/asset.hpp>

#include <memory>

namespace Asset {

    Asset::Asset(const File& file) 
        : files({file})
    {

    }
    Asset::Asset(const std::vector<File>& files) 
        : files(files)
    {

    }
    Asset::Asset(const Path& path) 
        : files({path})
    {

    }
    Asset::Asset(const std::vector<Path>& paths) 
    {
        for (const auto& path : paths) {
            if (!path.IsEmpty()) {
                files.emplace_back(path);
            }
        }
    }

    AssetManager::AssetManager()
    {
    }

    AssetManager& AssetManager::Instance() {
        static AssetManager instance;
        return instance;
    }

    void AssetManager::Initialize() {
        
    }

    template<class T, typename... Args>
    T& AssetManager::ImportAsset(fs::path path, bool hot_reload, Args&& ...args) {
        return ImportAsset({path}, hot_reload, args...);
    }

    template<class T, typename... Args>
    T& AssetManager::ImportAsset(const std::set<fs::path>& paths, bool hot_reload, Args&& ...args) {
        RegistryHandle id = assetRegistry.Push(std::make_shared<T>(args...));
        AssetMetadata metadata;
        metadata.id = id;
        metadata.type = extensionTable.at(paths.begin()->extension().string());
        for (const auto& path : paths) {
            if (hot_reload) {
                hotAssets[path] = metadata;
            } else {
                coldAssets[path] = metadata;
            }
        }
        return GetAsset<T>(*paths.begin());
    }

    template<class T>
    T& AssetManager::GetAsset(fs::path path) {
        RegistryHandle id;
        const auto it1 = coldAssets.find(path);
        if (it1 != coldAssets.end()) {
            RegistryHandle id = it1->second.id;
            return static_cast<T&>(assetRegistry.Get(id));
        } 
        const auto it2 = hotAssets.find(path);
        if (it2 != hotAssets.end()) {
            RegistryHandle id = it2->second.id;
            return static_cast<T&>(assetRegistry.Get(id));
        } 
        throw std::runtime_error("GetAsset: Asset not found!");
    }

    void AssetManager::RemoveAsset(fs::path path) {
        RegistryHandle id;
        const auto it1 = coldAssets.find(path);
        if (it1 != coldAssets.end()) {
            coldAssets.erase(it1);
            return;
        } 
        const auto it2 = hotAssets.find(path);
        if (it2 != hotAssets.end()) {
            hotAssets.erase(it2);
            return;
        } 
        throw std::runtime_error("RemoveAsset: Asset not found!");
    }

    AssetMetadata AssetManager::GetAssetMetadata(fs::path path) {
        return coldAssets.at(path);
    }

    void AssetManager::ColdSyncWithDevice() {
        syncWithDevice(true);
    }

    void AssetManager::HotSyncWithDevice() {
        syncWithDevice(false);
    }

    void AssetManager::syncWithDevice(bool sync_cold) {
        for (auto& [path, metadata] : hotAssets) {
            Asset& asset = assetRegistry.Get(metadata.id);
            asset.SyncWithDevice();
        }
        if (sync_cold) {
            for (auto& [path, metadata] : coldAssets) {
                Asset& asset = assetRegistry.Get(metadata.id);
                asset.SyncWithDevice();
            }
        }
    }
}