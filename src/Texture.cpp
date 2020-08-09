/*
#include "../include/Texture.hpp"
#include <stb_image.h>
#include <GL/glew.h>
#include <iostream>

#define CLEAR_ERRORS(error_variable) GLenum error_variable = glGetError();

#define OPENGL_ERROR(err, msg, code) \
    err = glGetError(); \
    if(err != GL_NO_ERROR) { \
        std::cout << "GL_ERROR: " << msg; \
        ##code; \
    }

#define ERROR(cond, msg, code) \
    if(##cond) { \
        std::cout << "ERROR: " << msg; \
        ##code; \
    } 


#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

using namespace std;

Texture::Texture()
: mId(0), mLoaded(false)
{}

#define LOAD_ERROR_RETURN stbi_image_free(data); return false;
bool Texture::Load(const std::string& path)
{
    if(mLoaded) return false;    

    // Load from file
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    ERROR(data == NULL, "Couldn't load texture from file: \""<< path <<"\".\n", LOAD_ERROR_RETURN);

    // Generate a texture on OpenGL, storing its Id   
    CLEAR_ERRORS(err);
    
    glGenTextures(1, &mId);
    ERROR(mId == 0, "Generating texture: id 0 returned.\n", LOAD_ERROR_RETURN);

    glBindTexture(GL_TEXTURE_2D, mId);
    OPENGL_ERROR(err, "Couldn't bind texture "<< mId <<".\n", LOAD_ERROR_RETURN);

    // Set the texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    OPENGL_ERROR(err, "Couldn't set wrapping settigns of texture "<< mId <<".\n", LOAD_ERROR_RETURN);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    OPENGL_ERROR(err, "Couldn't set filtering settigns of texture "<< mId <<".\n", LOAD_ERROR_RETURN);    

    // Load the texture into GPU, according to the number of channels
    // of the image (RGB or RGBA)
    if(channels <= 3){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else{
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    OPENGL_ERROR(err, "Couldn't load texture "<< mId <<" into GPU.\n", LOAD_ERROR_RETURN);

    glGenerateMipmap(GL_TEXTURE_2D);
    OPENGL_ERROR(err, "Couldn't generate mipmap of texture "<< mId <<".\n", LOAD_ERROR_RETURN);

    stbi_image_free(data);

    mLoaded = true;

    return true;
}

bool Texture::Bind(unsigned index)
{
    
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, mId);
    
    return true;    
}

Texture::~Texture()
{
    if(mLoaded){
        glDeleteTextures(1, &mId);
    }
}
*/
