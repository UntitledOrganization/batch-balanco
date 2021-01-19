#pragma once

#include "Result.hpp"
#include <string>

namespace sbb
{

    enum TextureFlags
    {
        TEXTURE_FILTER_NEAREST,
        TEXTURE_FILTER_LINEAR
    };

    class Texture
    {
    private:
        unsigned mId;
        bool mLoaded;
        int mWidth;
        int mHeight;

    public:
        Texture();

        unsigned GetId() { return mId; }
        int GetWidth() { return mWidth; }
        int GetHeight() { return mHeight; }

        Status ActivateAndBind(unsigned index);

        Status Bind();

        Status Load(const std::string &path, TextureFlags = TEXTURE_FILTER_NEAREST);

        Status Load(const unsigned char *data, int width, int height, int nChannels, TextureFlags = TEXTURE_FILTER_NEAREST);

        void Cleanup();

        ~Texture();
    };

} // namespace sbb
