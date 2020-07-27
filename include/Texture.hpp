#pragma once

#include "Result.hpp"
#include <string>

namespace sbb
{   
    
enum TextureFlag
{
    TEXTURE_FILTER_LINEAR,
    TEXTURE_FILTER_NEAREST,
};

class Texture
{
private:    
    unsigned mId;
    bool mLoaded;
public:
    Texture();
    unsigned GetId() { return mId; }
    
    Status ActivateAndBind(unsigned index);

    Status Bind();

    Status Load(const std::string& path, TextureFlag filter = TEXTURE_FILTER_LINEAR);

    void Cleanup();
    
    ~Texture();
};

}

