#pragma once

#include <plum/util/file.hpp>
#include <plum/util/registry.hpp>

#include <filesystem>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

namespace fs = std::filesystem;

// https://www.reddit.com/r/gameenginedevs/comments/18ved2y/how_to_use_handles_for_asset_management/

// https://www.reddit.com/r/gameenginedevs/comments/xwctxs/asset_manager_architecture_help/

// https://github.com/skypjack/entt/blob/master/src/entt/entity/registry.hpp

// from file: texture, model, shader
// not from file: meshes, materials, lights, scene node, etc.
// construct from: path string, path object, File 

namespace Asset {
    
    inline static const std::map<std::string, std::string> extensionTable = {
        {".png",    "Texture"},
        {".jpg",    "Texture"},
        {".jpeg",   "Texture"},
        {".hdr",    "Texture"},
        {".obj",    "Model"  },
        {".gltf",   "Model"  },
        {".3mf",    "Model"  },
        {".glsl",   "Program"},
        {".vs",     "Program"},
        {".fs",     "Program"},
        {".gs",     "Program"},
        {".vert",   "Program"},
        {".frag",   "Program"}
    };

    inline static const std::set<std::string> textureExtensions = {".png", ".jpg", ".jpeg", ".hdr"};
    inline static const std::set<std::string> modelExtensions = {".obj", ".gltf", ".3mf"};
    inline static const std::set<std::string> programExtensions = {".glsl", ".vs", ".fs", ".gs", ".vert", ".frag"};

    inline static std::string ExtensionToTypeName(std::string extension) {
        if (textureExtensions.find(extension) != textureExtensions.end()) {
            return "Texture";
        }
        if (modelExtensions.find(extension) != modelExtensions.end()) {
            return "Model";
        }
        if (programExtensions.find(extension) != programExtensions.end()) {
            return "Program";
        }
        return "";
    }

    class Asset {
        public:
            virtual ~Asset() = default;
            virtual void SyncWithDevice() = 0;
            // virtual Asset& CopyConfiguration(Path path) = 0;
            File GetFile() { return files[0]; }
            std::vector<File> GetFiles() { return files; }
            bool NeedsResync() const;
        protected:
            void syncFilesWithDevice();
            // friend class AssetManager;
            // Make sure to ignore empty paths
            Asset(const Path& path);
            Asset(const std::vector<Path>& paths);
            Asset(const File& file);
            Asset(const std::vector<File>& files);
            std::vector<File> files;
    };

    class AssetManager {
        public:
            static AssetManager& Instance();

            // static void Initialize();

            template<class T, typename... Args>
            std::shared_ptr<T> ImportAsset(Path path, bool hot_reload, Args&& ...args);

            template<class T, typename... Args>
            std::shared_ptr<T> ImportAsset(const std::vector<Path>& paths, bool hot_reload, Args&& ...args);
            
            template<class T>
            std::shared_ptr<T> GetAsset(Path path);
            
            void RemoveAsset(Path path);

            void ColdSyncWithDevice();
            void HotSyncWithDevice();

        private:
            struct AssetMetadata {
                RegistryHandle id;
                std::string type;
            };

            AssetManager() = default;

            void syncWithDevice(bool sync_cold = true);

            std::map<fs::path, AssetMetadata> coldAssets;
            std::map<fs::path, AssetMetadata> hotAssets;
            Registry<std::shared_ptr<Asset>> assetRegistry;
    };

    template<class T, typename... Args>
    std::shared_ptr<T> AssetManager::ImportAsset(Path path, bool hot_reload, Args&& ...args) {
        std::shared_ptr<T> object = std::make_shared<T>(args...);
        RegistryHandle id = assetRegistry.Push(std::static_pointer_cast<Asset>(object));
        AssetMetadata metadata;
        metadata.id = id;
        metadata.type = ExtensionToTypeName(path.Extension());
        if (hot_reload) {
            hotAssets[path.RawPath()] = metadata;
        } else {
            coldAssets[path.RawPath()] = metadata;
        }
        return object;
    }

    template<class T, typename... Args>
    std::shared_ptr<T> AssetManager::ImportAsset(const std::vector<Path>& paths, bool hot_reload, Args&& ...args) {
        std::shared_ptr<T> object = std::make_shared<T>(args...);
        RegistryHandle id = assetRegistry.Push(std::static_pointer_cast<Asset>(object));
        AssetMetadata metadata;
        metadata.id = id;
        metadata.type = ExtensionToTypeName(paths[0].Extension());
        for (const auto& path : paths) {
            if (hot_reload) {
                hotAssets[path.RawPath()] = metadata;
            } else {
                coldAssets[path.RawPath()] = metadata;
            }
        }
        return object;
    }

    template<class T>
    std::shared_ptr<T> AssetManager::GetAsset(Path path) {
        RegistryHandle id;
        const auto it1 = coldAssets.find(path.RawPath());
        if (it1 != coldAssets.end()) {
            RegistryHandle id = it1->second.id;
            return std::static_pointer_cast<T>(assetRegistry.Get(id));
        } 
        const auto it2 = hotAssets.find(path.RawPath());
        if (it2 != hotAssets.end()) {
            RegistryHandle id = it2->second.id;
            return std::static_pointer_cast<T>(assetRegistry.Get(id));
        } 
        throw std::runtime_error("GetAsset: Asset not found!");
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

}