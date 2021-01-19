#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string>
#include "Result.hpp"

#define GLCall(callFunc) \
    {                    \
        GLClearError();  \
        callFunc;        \
    }

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR)
        ;
}

static sbb::Status GLCheckError()
{
    while (GLenum error = glGetError())
    {
        const std::string errorString = std::string((char *)gluErrorString(error));
        return {sbb::ERROR_OPENGL, "OpenGL Error: " + errorString};
    }

    return {sbb::RESULT_OK, ""};
}
