#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <set>

namespace fs = std::filesystem;

// https://www.reddit.com/r/gameenginedevs/comments/18ved2y/how_to_use_handles_for_asset_management/

// https://www.reddit.com/r/gameenginedevs/comments/xwctxs/asset_manager_architecture_help/

// https://github.com/skypjack/entt/blob/master/src/entt/entity/registry.hpp

class Path {
    public:
        Path() = default;   // empty path
        Path(const char* path);
        Path(fs::path raw_path);
        
        fs::path RawPath() const { return path; }
        std::string Filename() const { return path.filename(); }
        std::string Name() const { return path.stem(); }
        std::string Extension() const { return path.extension(); }
        Path Parent() const { return Path(path.parent_path()); }
        fs::file_time_type LastModified() const { return time; }
        
        bool IsDirectory() const { return fs::is_directory(path); }
        bool IsEmpty() const { return path.empty(); }
        bool IsHidden() const;
        
        bool NeedsResync() const;
        virtual void SyncWithDevice();
        
        static Path CurrentPath() { return Path(fs::current_path()); }
        fs::path RelativePath(Path base = CurrentPath()) const { return fs::relative(path, base.path); }
        
        void Rename(fs::path name);
        void RenameAbsolute(fs::path abspath);

    
    protected:
        fs::path path;
        fs::file_time_type time;
};

class Directory : public Path {
    public:
        Directory(Path path);
        // Directory(fs::path path, const std::set<std::string>& extensions);

        std::vector<Path> List() const;
        std::vector<Path> ListRecursive() const;
        std::vector<Path> ListAll() const;
        std::vector<Path> ListAllRecursive() const;
        std::vector<Path> ListOnly(std::set<std::string> extensions) const;
        std::vector<Path> ListOnlyRecursive(std::set<std::string> extensions) const;

    private:
        // std::set<std::string> extensions;
    
    public:
        Directory(const Directory& d);
};

class File : public Path {
    public:
        // File();
        File(Path path);
        explicit File(fs::path path);

        std::uintmax_t Size() const { return size; }
        void SyncWithDevice() override;

        std::fstream& Open();
        void Close();
        
    private:
        std::fstream stream;
        std::uintmax_t size;

    public:
        File(const File& f);
};

// class FileExplorer {
// public:
//     FileExplorer();
// private:
// };