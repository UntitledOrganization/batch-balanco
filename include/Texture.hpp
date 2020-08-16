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

    public:
        Texture();
        unsigned GetId() {
            return mId;
        }

        Status ActivateAndBind(unsigned index);

        Status Bind();

        Status Load(const std::string& path, TextureFlags = TEXTURE_FILTER_NEAREST);

        void Cleanup();

        ~Texture();
    };

}

