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
    stbi_image_free(data8);
    stbi_image_free(data32);
}

const void* ImageAsset::Data8() {
    if (!data8) {
        stbi_set_flip_vertically_on_load(flip);
        data8 = stbi_load(file.RawPath().c_str(), &width, &height, &numChannels, 0);
    }
    return data8;
}

const void* ImageAsset::Data32() {
    if (!data32) {
        stbi_set_flip_vertically_on_load(flip);
        data32 = stbi_loadf(file.RawPath().c_str(), &width, &height, &numChannels, 0);
    }
    return data32;
}

void ImageAsset::syncWithFile() {
    stbi_set_flip_vertically_on_load(flip);
    if (data8) {
        stbi_image_free(data8);
        data8 = stbi_load(file.RawPath().c_str(), &width, &height, &numChannels, 0);
    }
    if (data32) {
        stbi_image_free(data32);
        data32 = stbi_loadf(file.RawPath().c_str(), &width, &height, &numChannels, 0);
    }
}