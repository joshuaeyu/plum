#pragma once

#include "asset/manager.hpp"

class ImageAsset : public Asset {
    public:
        ImageAsset(const Path& path, bool flip_uv = true);
        ~ImageAsset();
        
        bool Flip() const { return flip; }
        bool IsHdr() const { return hdr; }
        int Width() const { return width; }
        int Height() const { return height; }
        int NumChannels() const { return numChannels; }
        
        void SetFlip(bool flip_uv) { flip = flip_uv; }

        const void* Data8();
        const void* Data32();

    private:
        bool hdr;
        bool flip;
        int width, height;
        int numChannels;
        void* data8 = nullptr;
        void* data32 = nullptr;

        void syncWithFile() override;
};