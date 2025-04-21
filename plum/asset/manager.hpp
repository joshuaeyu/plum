#pragma once

#include <plum/asset/asset.hpp>

#include <plum/util/file.hpp>
#include <plum/util/registry.hpp>

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

        // static void Initialize();
        
        template<class T, typename... Args> 
        std::shared_ptr<T> LoadHot(const Path& path, Args&& ...args);
        template<class T, typename... Args> 
        std::shared_ptr<T> LoadCold(const Path& path, Args&& ...args);
        
        template<class T>
        std::shared_ptr<T> Get(const Path& path);

        void Remove(const Path& path);

        template<class T>
        const std::vector<std::shared_ptr<T>>& GetAllOfType();

        void ColdSyncWithDevice();
        void HotSyncWithDevice();

    private:
        struct AssetInfo {
            std::shared_ptr<Asset> asset;
            std::type_index type = std::type_index(typeid(int));
            bool hotReload;
        };

        AssetManager() = default;
        
        template<class T, typename... Args> 
        std::shared_ptr<T> load(bool hot_reload, const Path& path, Args&& ...args);
        
        void syncWithDevice(bool sync_cold = true);

        std::map<fs::path, AssetInfo> assets;
        // Registry<std::shared_ptr<Asset>> assetRegistry;
        // std::map<std::type_index, Registry<std::shared_ptr<Asset>>> registries;
        // inheritance tree?
        // how to handle inheritance? std::is_base_of
        // - want all materials, which includes

        // Resource manager requirements
        // - store external assets (models, images, shaders) and reload on demand
        // - store internal assets (materials, texes, meshes)
        // - assets may reference each other!
        // - list assets of a given type - use type index!!!!!!

        // need to access by id but also path
};

template<class T, typename... Args>
std::shared_ptr<T> AssetManager::LoadHot(const Path& path, Args&& ...args) {
    return load<T>(true, path, args...);
}

template<class T, typename... Args>
std::shared_ptr<T> AssetManager::LoadCold(const Path& path, Args&& ...args) {
    return load<T>(false, path, args...);
}

template<class T>
std::shared_ptr<T> AssetManager::Get(const Path& path) {
    if (!std::is_base_of<Asset, T>::value) {
        throw std::invalid_argument("Object type must inherit from Asset!");
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
        throw std::invalid_argument("Object type must inherit from Asset!");
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
std::shared_ptr<T> AssetManager::load(bool hot_reload, const Path& path, Args&& ...args) {
    if (!std::is_base_of<Asset, T>::value) {
        throw std::invalid_argument("Object type must inherit from Asset!");
    }
    std::shared_ptr<T> asset = Get<T>(path);
    if (asset) {
        std::cout << "Warning - Attempting to emplace an existing asset!" << std::endl;
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


// Features
// - load all files in a directory
//      - need to know how to construct objects
//      - or... can call some callback? Asset::ReconstructInPlace(path) or Asset::ConstructWithCurrentSettings(path)
// - path -> metadata struct -> actual data

// registry: id -> asset
// assetmanager: path -> file metadata -> asset

// Loads assets from path and stores in registry (i.e., cache)

// Scans directory in real time - i.e., reflects the state of the dir and not what the user has given the assetmanager
// Means to "reinitialize" each of the assets (Texture, Model, Program) using their current file system representation
// 1. List all files
//      per subdirectory? or just a full on explorer?
// 2. Not all files are loaded in cache (imported) yet
//      Files loaded in cache are.....? Labeled?
//      Passively monitor cached files for updates and update at some frequency or on demand?
// 3. Available 

// Option 1: Asset Explorer section AND individual imported asset sections

// Models used when
// - creating SceneNode with a Component
// - adding a component to an existing SceneNode

// Textures used when
// - creating / modifying Materials


// Shaders used when
// - no direct interaction from GUI; should just cold/hot reload
// - AssetManager handles these differently?
// - Should AssetManager even handle these or some FileDaemon?

// AssetManager manages files. I.e., Models, Textures, Programs
//  - allows GUI users to import and actually use external assets!
//  - separate from FileExplorer.
// Standalone Registry needed for objects. I.e., Materials, (Meshes/VAOs/VBOs/EBOs/VertexArrays?, Components, SceneNode prefabs)
// Serialization needed for saving imported Assets and Scene.


// Workflow
// 1. Asset Explorer shows a basic file explorer of Assets/
// 2. Files can be imported into the demo/scene's "Imported Assets" (Registry<Asset>) through the Asset Explorer
//      Right click -> "Texture" - Import (based on extension) -> Popup with import options
// 3. Models, textures, etc. can be used to create components and materials in the scene.
// 4. SceneNodes and their Components can then be instantiated in the scene.

// Application user
// 1. Interact with file system and see updates in real time
// 2. Import models, textures
// 3. Create materials from textures
// 4. Create scene nodes with model components

// API user - using file system assets
// 1. Import models, textures into AssetManager
// 2. Create materials from textures cached by the AssetManager
// 4. Create scene nodes from models cached by the AssetManager
// 3. Create lights, primitives, etc. components directly
// 5. Create scene nodes from standalone lights, primitves

// API user - 
// 1. Directly create model, texture objects
// 2. Create materials from standalone texture objects
// 3. Create scene nodes from standalone model objects
// 4. Create lights, primitives, etc. components directly
// 5. Create scene nodes from standalone lights, primitves

// ModelFile -> Model
// ImageFile -> Texture
// ShaderFile -> Program
// These are hashed based on path

// Files are stored in an AssetFileManager

// Assets are stored in an AssetManager
// Internal assets aren't automatically hashed

// RawModel, RawImage, RawShader
// ModelData, ImageData, ShaderData

// Internal Assets