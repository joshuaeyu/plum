#include <plum/asset/image.hpp>

#include <plum/util/file.hpp>

#include <stb/stb_image.h>

#include <stdexcept>

ImageAsset::ImageAsset(const Path& path, bool flip)
    : Asset(path),
    flip(flip)
{
    hdr = stbi_is_hdr(path.RawPath().c_str());
    if (stbi_info(path.RawPath().c_str(), &width, &height, &numChannels) != 1) {
        throw std::runtime_error("Image type not supported!");
    }
}

ImageAsset::~ImageAsset() {
    stbi_image_free(data);
}

const void* ImageAsset::Data() {
    if (!data) {
        stbi_set_flip_vertically_on_load(flip);
        if (hdr)
            data = stbi_loadf(file.RawPath().c_str(), &width, &height, &numChannels, 0);
        else
            data = stbi_load(file.RawPath().c_str(), &width, &height, &numChannels, 0);
    }
    return data;
}

void ImageAsset::SyncWithFile() {
    stbi_set_flip_vertically_on_load(flip);
    if (hdr)
        data = stbi_loadf(file.RawPath().c_str(), &width, &height, &numChannels, 0);
    else
        data = stbi_load(file.RawPath().c_str(), &width, &height, &numChannels, 0);
}