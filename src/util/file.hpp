#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <set>

namespace fs = std::filesystem;

class Path {
    public:
        Path() = default;   // empty path
        Path(const Path& path);
        Path(const char* path);
        Path(const fs::path& raw_path);
        
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
        
        static Path CurrentPath() { return fs::current_path(); }
        fs::path RelativePath(const Path& base = CurrentPath()) const { return fs::relative(path, base.path); }
        
        void Rename(const fs::path& name);
        void RenameAbsolute(const fs::path& abspath);
    
    protected:
        fs::path path;
        fs::file_time_type time;
};

class Directory : public Path {
    public:
        Directory(const Directory& d);        
        Directory(const Path& path);

        std::vector<Path> List() const;
        std::vector<Path> ListRecursive() const;
        std::vector<Path> ListAll() const;
        std::vector<Path> ListAllRecursive() const;
        std::vector<Path> ListOnly(const std::set<std::string>& extensions) const;
        std::vector<Path> ListOnlyRecursive(const std::set<std::string>& extensions) const;
};

class File : public Path {
    public:
        File(const File& f);
        File(const Path& path);
        explicit File(const fs::path& path);

        std::uintmax_t Size() const { return size; }
        void SyncWithDevice() override;

        std::fstream& Open();
        void Close();
        
    private:
        std::fstream stream;
        std::uintmax_t size;
};