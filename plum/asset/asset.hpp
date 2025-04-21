#pragma once

#include <plum/util/file.hpp>

#include <memory>
#include <set>
#include <vector>

// Turn into a plain callback later
class AssetUser {
    public:
        virtual void AssetResyncCallback() = 0;  
    protected:
        AssetUser() = default;
};

class Asset {
    public:
        virtual ~Asset() = default;
        // virtual Asset& CopyConfiguration(Path path) = 0;
        const File& GetFile() const { return file; } 
        bool NeedsResync() const;
        
        void Resync();
        virtual void SyncWithFile() = 0;
        
        void AddUser(AssetUser* user);
        void RemoveUser(AssetUser* user);
        
        std::set<AssetUser*> users;
        
    protected:
        // friend class AssetManager;
        // Make sure to ignore empty paths
        Asset(const Path& path);
        Asset(const std::vector<Path>& paths);
        Asset(const File& file);
        Asset(const std::vector<File>& files);
        File file;
};
