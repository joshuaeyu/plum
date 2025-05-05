#pragma once

#include "asset/asset.hpp"

#include "util/file.hpp"

#include <iostream>
#include <filesystem>
#include <map>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <type_traits>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

// https://www.reddit.com/r/gameenginedevs/comments/18ved2y/how_to_use_handles_for_asset_management/

// https://www.reddit.com/r/gameenginedevs/comments/xwctxs/asset_manager_architecture_help/

// https://github.com/skypjack/entt/blob/master/src/entt/entity/registry.hpp

// from file: texture, model, shader
// not from file: meshes, materials, lights, scene node, etc.
// construct from: path string, path object, File 

namespace AssetUtils {
    
    const std::set<std::string> imageExtensions = {".png", ".jpg", ".jpeg", ".hdr"};
    const std::set<std::string> modelExtensions = {".obj", ".gltf", ".3mf"};
    const std::set<std::string> shaderExtensions = {".glsl", ".vs", ".fs", ".gs", ".vert", ".frag"};

    inline static std::string ExtensionToTypeName(std::string extension) {
        if (imageExtensions.count(extension) > 0) {
            return "Image";
        }
        if (modelExtensions.count(extension) > 0) {
            return "Model";
        }
        if (shaderExtensions.count(extension) > 0) {
            return "Shader";
        }
        return "";
    }
}

class AssetManager {
    public:
        static AssetManager& Instance();
        
        template<class T>
        std::shared_ptr<T> Get(const Path& path);
        template<class T>
        const std::vector<std::shared_ptr<T>>& GetAllOfType();
        
        template<class T, typename... Args> 
        std::shared_ptr<T> LoadHot(const Path& path, Args&& ...args);
        template<class T, typename... Args> 
        std::shared_ptr<T> LoadCold(const Path& path, Args&& ...args);
    
        void Remove(const Path& path);

        void ColdSyncWithDevice();
        void HotSyncWithDevice();

    private:
        struct AssetInfo {
            std::shared_ptr<Asset> asset;
            std::type_index type = std::type_index(typeid(int));
            bool hotReload;
        };

        AssetManager() = default;
        
        std::map<fs::path, AssetInfo> assets;
  
        template<class T, typename... Args> 
        std::shared_ptr<T> load(bool hot_reload, const Path& path, Args&& ...args);
        
        void syncWithDevice(bool sync_cold = true);

};

template<class T>
std::shared_ptr<T> AssetManager::Get(const Path& path) {
    if (!std::is_base_of<Asset, T>::value) {
        throw std::runtime_error("Object type must inherit from Asset!");
    }
    const auto it1 = assets.find(path.RawPath());
    if (it1 != assets.end()) {
        return std::static_pointer_cast<T>(it1->second.asset);
    } 
    return nullptr;
}

template<class T>
const std::vector<std::shared_ptr<T>>& AssetManager::GetAllOfType() {
    std::vector<std::shared_ptr<T>> result;
    if (!std::is_base_of<Asset, T>::value) {
        throw std::runtime_error("Object type must inherit from Asset!");
    }
    std::type_index type(std::type_index(typeid(T)));
    for (const auto& [_, info] : assets) {
        if (type == info.type) {
            result.emplace_back(std::static_pointer_cast<T>(info.asset));
        }
    }
    return result;
}

template<class T, typename... Args>
std::shared_ptr<T> AssetManager::LoadHot(const Path& path, Args&& ...args) {
    return load<T>(true, path, args...);
}

template<class T, typename... Args>
std::shared_ptr<T> AssetManager::LoadCold(const Path& path, Args&& ...args) {
    return load<T>(false, path, args...);
}

template<class T, typename... Args> 
std::shared_ptr<T> AssetManager::load(bool hot_reload, const Path& path, Args&& ...args) {
    if (!std::is_base_of<Asset, T>::value) {
        throw std::runtime_error("Object type must inherit from Asset!");
    }
    std::shared_ptr<T> asset = Get<T>(path);
    if (asset) {
        // std::clog << "Info - Loading an asset that was already loaded!" << std::endl;
    } else {
        asset = std::make_shared<T>(path, std::forward<Args>(args)...);
        
        AssetInfo info {
            asset,
            std::type_index(typeid(T)),
            hot_reload
        };

        assets[path.RawPath()] = info;
    }   
    return asset;
}