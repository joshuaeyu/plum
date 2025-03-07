#ifndef IMPORTER_HPP
#define IMPORTER_HPP

#include <filesystem>

class Importer {
    public:
        void Load(std::string path);
        void Load(std::filesystem::path path);
    protected:
        Importer();
};

class ModelImporter : public Importer {
    // Assimp stuff
};

class TextureImporter : public Importer {
    // stbi stuff

};

// raw file/dir -> Import -> serialized -> Load/Use

#endif