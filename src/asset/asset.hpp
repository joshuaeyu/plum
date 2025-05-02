#pragma once

#include "util/file.hpp"

#include <memory>
#include <set>
#include <vector>

// Turn into a plain callback later
class AssetUser {
    public:
        virtual ~AssetUser() = default;
        virtual void AssetResyncCallback() = 0;  
    
    protected:
        AssetUser() = default;
};

class Asset {
    public:
        virtual ~Asset() = default;

        const File& GetFile() const { return file; } 
        bool NeedsResync() const;
        
        void Resync();
        void AddUser(AssetUser* user);
        void RemoveUser(AssetUser* user);
            
    protected:
        Asset() = delete;
        Asset(const Path& path);
        Asset(const std::vector<Path>& paths);
        Asset(const File& file);
        Asset(const std::vector<File>& files);
        
        File file;
        std::set<AssetUser*> users;

        virtual void syncWithFile() = 0;
};
