#pragma once

#include <string>

class Texture
{
private:    
    unsigned mId;
    bool mLoaded;
public:
    Texture();
    unsigned GetId() { return mId; }
    bool Bind(unsigned index);
    bool Load(const std::string& path);
    ~Texture();
};