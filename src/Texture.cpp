#include "../include/Texture.hpp"
#include "../include/GLCall.hpp"
#include <GL/glew.h>
#include <string>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

namespace sbb
{

    Texture::Texture()
        : mId(0), mLoaded(false)
    {
    }

    Status Texture::Load(const unsigned char *data, int width, int height, int nChannels, TextureFlags flags)
    {
        if (mLoaded)
            return {ERROR_ALREADY_LOADED, "Texture already loaded."};

        if (nChannels != 3 && nChannels != 4)
            return {ERROR_TEXTURE, "Texture has unsupported format."};

        Status glStatus;

        // Generate and bind a texture on OpenGL, storing its Id
        glGenTextures(1, &mId);
        glBindTexture(GL_TEXTURE_2D, mId);

        // Set the texture wrapping options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Set the texture filtering options
        GLenum filterEnum;
        if (flags == TEXTURE_FILTER_NEAREST)
            filterEnum = GL_NEAREST;
        else
            filterEnum = GL_LINEAR;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterEnum);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterEnum);

        // Load the texture into GPU, according to the number of channels
        // of the image (RGB or RGBA)
        if (nChannels == 3)
        {
            GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
            if (glStatus = GLCheckError(), !glStatus)
            {
                Cleanup();
                return {ERROR_OPENGL, "Couldn't load texture in GPU. " + glStatus.message};
            }
        }
        else if (nChannels == 4)
        {
            GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
            if (glStatus = GLCheckError(), !glStatus)
            {
                Cleanup();
                return {ERROR_OPENGL, "Couldn't load texture in GPU. " + glStatus.message};
            }
        }

        // Generate mipmap
        glGenerateMipmap(GL_TEXTURE_2D);

        mWidth = width;
        mHeight = height;
        mLoaded = true;

        return {RESULT_OK, ""};
    }

    Status Texture::Load(const std::string &path, TextureFlags flags)
    {
        if (mLoaded)
            return {ERROR_ALREADY_LOADED, "Texture already loaded."};

        // Get data from file
        stbi_set_flip_vertically_on_load(true);
        int width, height, nChannels;
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &nChannels, 0);
        if (data == NULL)
            return {ERROR_READ_FILE, "Couldn't get texture data from file: \"" + path + "\"."};

        // Load from data
        Status loadStatus = Load(data, width, height, nChannels, flags);
        stbi_image_free(data);

        return loadStatus;
    }

    Status Texture::ActivateAndBind(unsigned index)
    {

        Status glStatus;

        // Activate texture unit "index"
        GLCall(glActiveTexture(GL_TEXTURE0 + index));
        if (glStatus = GLCheckError(), !glStatus)
            return {ERROR_OPENGL, "Couldn't activate texture unit " + std::to_string(index) + ". " + glStatus.message};

        // Bind texture
        GLCall(glBindTexture(GL_TEXTURE_2D, mId));
        if (glStatus = GLCheckError(), !glStatus)
            return {ERROR_OPENGL, "Couldn't bind texture " + std::to_string(mId) + ". " + glStatus.message};

        return {RESULT_OK, ""};
    }

    Status Texture::Bind()
    {
        Status glStatus;

        // Bind texture
        GLCall(glBindTexture(GL_TEXTURE_2D, mId));
        if (glStatus = GLCheckError(), !glStatus)
            return {ERROR_OPENGL, "Couldn't bind texture " + std::to_string(mId) + ". " + glStatus.message};

        return {RESULT_OK, ""};
    }

    void Texture::Cleanup()
    {
        glDeleteTextures(1, &mId);
        mId = 0;
        mWidth = 0;
        mHeight = 0;
        mLoaded = false;
    }

    Texture::~Texture()
    {
    }

} // namespace sbb
