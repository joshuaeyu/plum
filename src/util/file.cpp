#include "scene/all.hpp"

Path::Path(const Path& path)
    : path(path.path),
    time(path.time)
{}

Path::Path(const char* path)
    : Path(fs::path(path))
{}

Path::Path(const fs::path& raw_path)
{
    if (fs::path(raw_path).is_absolute()) {
        path = raw_path;
    } else {
        path = fs::current_path() / raw_path;
    }
    time = fs::last_write_time(path);
}

bool Path::IsHidden() const {
    std::string name = path.filename();
    return name == ".." || name == "." || name[0] == '.';
}

bool Path::NeedsResync() const {
    return fs::last_write_time(path) != time;
}

void Path::SyncWithDevice() {
    time = fs::last_write_time(path);
}

void Path::Rename(const fs::path& name) {
    fs::path newPath = path.parent_path() / name;
    fs::rename(path, newPath);
    path = newPath;
}

void Path::RenameAbsolute(const fs::path& abs_path) {
    fs::rename(path, abs_path);
    path = abs_path;
}

Directory::Directory(const Path& path)
    : Path(path)
{
    if (!IsDirectory() || IsEmpty()) {
        throw std::runtime_error("Path must be a directory!");
    }
}

Directory::Directory(const Directory& d)
    : Path(d.path)
{}

std::vector<Path> Directory::List() const {
    std::vector<Path> children;
    for (const auto& entry : fs::directory_iterator(path)) {
        if (!Path(entry.path()).IsHidden())
            children.emplace_back(entry);
    }
    return children;
}

std::vector<Path> Directory::ListOnly(const std::set<std::string>& extensions) const {
    std::vector<Path> children;
    for (const auto& entry : fs::directory_iterator(path)) {
        if (extensions.find(Path(entry).Extension()) != extensions.end())
            children.emplace_back(entry);
    }
    return children;
}

std::vector<Path> Directory::ListOnlyRecursive(const std::set<std::string>& extensions) const {
    std::vector<Path> children;
    for (const auto& entry : fs::recursive_directory_iterator(path)) {
        if (extensions.find(Path(entry).Extension()) != extensions.end())
            children.emplace_back(entry);
    }
    return children;
}

std::vector<Path> Directory::ListAll() const {
    std::vector<Path> children;
    for (const auto& entry : fs::directory_iterator(path)) {
        children.emplace_back(entry);
    }
    return children;
}

std::vector<Path> Directory::ListRecursive() const {
    std::vector<Path> children;
    for (const auto& entry : fs::recursive_directory_iterator(path)) {
        if (!Path(entry.path()).IsHidden()) {
            children.emplace_back(entry);
        }
    }
    return children;
}

std::vector<Path> Directory::ListAllRecursive() const {
    std::vector<Path> children;
    for (const auto& entry : fs::recursive_directory_iterator(path)) {
        children.emplace_back(entry);
    }
    return children;
}

File::File(const Path& path)
    : Path(path)
{
    if (IsDirectory() || IsEmpty()) {
        throw std::runtime_error("Path cannot be a directory!");
    }
    size = fs::file_size(this->path);
}

File::File(const fs::path& raw_path)
    : Path(raw_path),
    size(fs::file_size(raw_path))
{
    if (IsDirectory() || IsEmpty()) {
        throw std::runtime_error("Path cannot be a directory!");
    }
    size = fs::file_size(path);
}

File::File(const File& f)
    : Path(f.path),
    size(f.size)
{}

void File::SyncWithDevice() {
    time = fs::last_write_time(path);
    size = fs::file_size(path);
}

std::fstream& File::Open() {
    stream.open(path, std::fstream::in | std::fstream::out);
    return stream;
}

void File::Close() {
    stream.close();
}