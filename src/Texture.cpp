#include "../include/Texture.hpp"
#include "../include/Logger.hpp"
#include <GL/glew.h>
#include <string>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

namespace sbb {

Texture::Texture()
: mId(0), mLoaded(false)
{}

#define LOAD_ERROR_RETURN ; return false;
Status Texture::Load(const std::string& path, TextureFlags flags)
{
    if(mLoaded) return {ERROR_ALREADY_LOADED, "Texture already loaded."};    

    // Load from file
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if(data == NULL)
        return {ERROR_READ_FILE, "Couldn't load texture from file: \""+ path +"\"."};

    
    
    // Reset error flag
    GLenum error = glGetError();
    
    // Generate and bind a texture on OpenGL, storing its Id   
    glGenTextures(1, &mId);
    glBindTexture(GL_TEXTURE_2D, mId);

    // Set the texture wrapping options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if(error = glGetError(), error != GL_NO_ERROR){
        stbi_image_free(data);
        Cleanup();
        return {ERROR_OPENGL, "Couldn't set texture wrapping options."};
    } 
        
    // Set the texture filtering options
    GLenum filterEnum;
    if(flags == TEXTURE_FILTER_NEAREST) filterEnum = GL_NEAREST;
    else filterEnum = GL_LINEAR;    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterEnum);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterEnum);
    if(error = glGetError(), error != GL_NO_ERROR){
        stbi_image_free(data);
        Cleanup();
        return {ERROR_OPENGL, "Couldn't set texture filtering options."};
    } 

    // Load the texture into GPU, according to the number of channels
    // of the image (RGB or RGBA)
    if(channels <= 3){        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else{
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    if(error = glGetError(), error != GL_NO_ERROR){
        stbi_image_free(data);
        Cleanup();
        return {ERROR_OPENGL,  "Couldn't load texture into GPU."};
    }

    // Generate mipmap
    glGenerateMipmap(GL_TEXTURE_2D);
    if(error = glGetError(), error != GL_NO_ERROR){
        stbi_image_free(data);
        Cleanup();
        return {ERROR_OPENGL,  "Couldn't generate texture mipmap."};
    }

    stbi_image_free(data);
    mLoaded = true;

    return {RESULT_OK, ""};
}

Status Texture::ActivateAndBind(unsigned index)
{
    // Reset error flag
    GLenum error = glGetError();

    // Activate texture unit "index"
    glActiveTexture(GL_TEXTURE0 + index);
    if(error = glGetError(), error != GL_NO_ERROR){
        return {ERROR_OPENGL,  "Couldn't activate texture unit "+std::to_string(index)+". Index out of range."};
    }

    // Bind texture
    glBindTexture(GL_TEXTURE_2D, mId);
    if(error = glGetError(), error != GL_NO_ERROR){
        return {ERROR_OPENGL,  "Couldn't bind texture "+ std::to_string(mId) +"."};
    }

    return {RESULT_OK, ""};
}

Status Texture::Bind()
{   
    // Reset error flag
    GLenum error = glGetError();
    
    // Bind texture
    glBindTexture(GL_TEXTURE_2D, mId);
    if(error = glGetError(), error != GL_NO_ERROR){
        return {ERROR_OPENGL,  "Couldn't bind texture "+ std::to_string(mId) +"."};
    }

    return {RESULT_OK, ""};
}

void Texture::Cleanup()
{    
    glDeleteTextures(1, &mId);
    mId = 0;
    mLoaded = false;
}

Texture::~Texture()
{}

}

