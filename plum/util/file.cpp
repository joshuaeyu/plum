#include <plum/util/file.hpp>

namespace fs = std::filesystem;

Path::Path(const char* path) 
    : Path(fs::path(path))
{}

Path::Path(fs::path path)
    : Path(path, fs::current_path())
{}

Path::Path(fs::path path, fs::path abs_dir)
    : path(path),
    base(abs_dir),
    time(fs::last_write_time(path))
{}

void Path::Rename(fs::path name) {
    fs::path newPath(path.parent_path() / name);
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

Directory::Directory(fs::path raw_path, fs::path raw_abs_dir)
    : Path(raw_path, raw_abs_dir)
{
    if (!IsDirectory() || IsEmpty()) {
        throw std::runtime_error("Path must be a directory!");
    }
}

Directory::Directory(const Directory& d)
    : Path(d.path, d.base)
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

File::File(fs::path raw_path, fs::path raw_abs_dir)
    : Path(raw_path, raw_abs_dir)
{
    if (IsDirectory() || IsEmpty()) {
        throw std::runtime_error("Path cannot be a directory!");
    }
    size = fs::file_size(path);
}

File::File(const File& f)
    : Path(f.path, f.base),
    size(f.size)
{}

std::fstream& File::Open() {
    stream.open(path, std::fstream::in | std::fstream::out);
    return stream;
}

void File::Close() {
    stream.close();
}



// texture, model, shader
// construct from: path string, path object, File 