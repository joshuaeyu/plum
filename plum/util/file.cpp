#include <plum/util/file.hpp>

namespace fs = std::filesystem;

Path::Path(const char* path)
    : Path(fs::path(path))
{}

Path::Path(fs::path path)
{
    if (fs::path(path).is_absolute()) {
        this->path = path;
    } else {
        this->path = fs::current_path() / path;
    }
    time = fs::last_write_time(path);
}

bool Path::NeedsResync() const {
    return fs::last_write_time(path) != time;
}

void Path::SyncWithDevice() {
    time = fs::last_write_time(path);
}

void Path::Rename(fs::path name) {
    fs::path newPath = path.parent_path() / name;
    fs::rename(path, newPath);
    path = newPath;
}

void Path::RenameAbsolute(fs::path abspath) {
    fs::rename(path, abspath);
    path = abspath;
}

Directory::Directory(Path path)
    : Path(path)
{
    if (!IsDirectory() || IsEmpty()) {
        throw std::runtime_error("Path must be a directory!");
    }
}

Directory::Directory(fs::path raw_path)
    : Path(raw_path)
{
    if (!IsDirectory() || IsEmpty()) {
        throw std::runtime_error("Path must be a directory!");
    }
}

Directory::Directory(const Directory& d)
    : Path(d.path)
{}

// std::vector<fs::path> Directory::ListFiltered() const {
//     std::vector<fs::path> files;
//     for (const auto& entry : fs::recursive_directory_iterator(path)) {
//         if (extensions.find(entry.path().extension()) != extensions.end()) {
//             files.push_back(fs::relative(entry.path(), path));
//         }
//     }
//     return files;
// }

std::vector<fs::path> Directory::List() const {
    std::vector<fs::path> files;
    for (const auto& entry : fs::directory_iterator(path)) {
        files.push_back(fs::relative(entry.path(), path));
    }
    return files;
}

std::vector<fs::path> Directory::ListRecursive() const {
    std::vector<fs::path> files;
    for (const auto& entry : fs::recursive_directory_iterator(path)) {
        files.push_back(fs::relative(entry.path(), path));
    }
    return files;
}

File::File(Path path)
    : Path(path)
{
    if (IsDirectory() || IsEmpty()) {
        throw std::runtime_error("Path cannot be a directory!");
    }
    size = fs::file_size(this->path);
}

File::File(fs::path raw_path)
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



// texture, model, shader
// construct from: path string, path object, File 