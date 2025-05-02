#include "asset/asset.hpp"

Asset::Asset(const File& file) 
    : file(file)
{}

Asset::Asset(const Path& path) 
    : file(path)
{}

bool Asset::NeedsResync() const {
    return file.NeedsResync();
}

void Asset::Resync() {
    file.SyncWithDevice();
    syncWithFile();
    for (const auto& user : users) {
        user->AssetResyncCallback();
    }
}
            
void Asset::AddUser(AssetUser* user) {
    users.insert(user);
}

void Asset::RemoveUser(AssetUser* user) {
    users.erase(user);
}