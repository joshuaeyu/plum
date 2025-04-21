#pragma once

#include <plum/asset/manager.hpp>

class ImageAsset : public Asset {
    public:
        ImageAsset(const Path& path, bool flip = true);
        ~ImageAsset();
        
        bool IsHdr() const { return hdr; }
        int Width() const { return width; }
        int Height() const { return height; }
        int NumChannels() const { return numChannels; }

        const void* Data();

        void SyncWithFile() override;

    private:
        void* data = nullptr;
        bool hdr;
        bool flip;
        int width, height;
        int numChannels;
};