#include "../include/OpenGLShader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace sbb
{

    OpenGLShader::OpenGLShader(
        const std::string& vertexShaderFilePath,
        const std::string& fragmentShaderFilePath)
        : Shader(vertexShaderFilePath, fragmentShaderFilePath),
        mcompilationSource(ShaderSourceType::SHADER_FILES),
        mshaderProgram(0)
    { }

    OpenGLShader::OpenGLShader(const ShaderSources& source)
        :	Shader(source),
        mcompilationSource(ShaderSourceType::SHADER_SOURCES),
        mshaderProgram(0)
    {}


    Status OpenGLShader::Init()
    {
        if (mcompilationSource == ShaderSourceType::SHADER_FILES)
        {
            auto [sources, status] = ExtractShaderSources();

            if (!status) return status;
            if (!status) return status;

            // TODO: Maybe using std::move to avoid this copy?
            mSources = sources;
        }

        return CompileShaders();
    }

    void OpenGLShader::Cleanup()
    {
        glDeleteProgram(mshaderProgram);
    }

    Status OpenGLShader::SetMat4(const char* name, const void* data)
    {
        GLint uniformLocation = GetUniformLocation(name);
        if (uniformLocation == -1) return { ERROR_SHADER, "Unable to find uniform" };

        glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, (float *)data);
        return { RESULT_OK };
    }

    Status OpenGLShader::SetVec4(const char* name, const Vec4& data)
    {
        GLint uniformLocation = GetUniformLocation(name);
        if (uniformLocation == -1) return { ERROR_SHADER, "Unable to find uniform" };

        glUniform4fv(uniformLocation, 1, (float *)(&data));
        return { RESULT_OK };
    }
    Status OpenGLShader::SetVec3(const char* name, const Vec3& data)
    {
        GLint uniformLocation = GetUniformLocation(name);
        if (uniformLocation == -1) return { ERROR_SHADER, "Unable to find uniform" };

        glUniform3fv(uniformLocation, 1, (float *)(&data));
        return { RESULT_OK };
    }

    Status OpenGLShader::SetVec2(const char* name, const Vec2& data)
    {
        GLint uniformLocation = GetUniformLocation(name);
        if (uniformLocation == -1) return { ERROR_SHADER, "Unable to find uniform" };

        glUniform2fv(uniformLocation, 1, (float *)(&data));
        return { RESULT_OK };
    }

    Status OpenGLShader::SetInt(const char* name, const int& data)
    {
        GLint uniformLocation = GetUniformLocation(name);
        if (uniformLocation == -1) return { ERROR_SHADER, "Unable to find uniform" };

        glUniform1i(uniformLocation, data);
        return { RESULT_OK };
    }

    Status OpenGLShader::SetFloat(const char* name, const float& data)
    {
        GLint uniformLocation = GetUniformLocation(name);
        if (uniformLocation == -1) return { ERROR_SHADER, "Unable to find uniform" };

        return { RESULT_OK };
    }

    Result<ShaderSources> OpenGLShader::ExtractShaderSources() const
    {
        std::fstream vsFileStream;
        std::fstream fsFileStream;

        vsFileStream.open(mVertexShaderFilePath, std::fstream::in);
        fsFileStream.open(mFragmentShaderFilePath, std::fstream::in);

        if (!vsFileStream) return {
            {},
            { ERROR_READ_FILE, "Unable to  open vertex shader file: " + mVertexShaderFilePath } };
        if (!fsFileStream) return {
            {},
            { ERROR_READ_FILE, "Unable to  open fragment shader file: " + mFragmentShaderFilePath } };

        std::stringstream vsStreamSource;
        std::stringstream fsStreamSource;

        vsStreamSource << vsFileStream.rdbuf();
        fsStreamSource << fsFileStream.rdbuf();

        vsFileStream.close();
        fsFileStream.close();

        return {
            { vsStreamSource.str(), fsStreamSource.str() },
            { RESULT_OK } };
    }


    Status OpenGLShader::CompileShaders()
    {
        GLuint vertexShader = CompileShader(mSources.vertexShader, ShaderType::VERTEX_SHADER);
        if (vertexShader == 0) return { ERROR_SHADER, "Unable to compile vertex shader." };

        GLuint fragmentShader = CompileShader(mSources.fragmentShader, ShaderType::FRAGMENT_SHADER);
        if (fragmentShader == 0)
        {
            glDeleteShader(vertexShader);
            return { ERROR_SHADER, "Unable to compile fragment shader." };
        }

        mshaderProgram = glCreateProgram();
        if (mshaderProgram == 0) return { ERROR_SHADER, "Unable to create shader program." };

        glAttachShader(mshaderProgram, vertexShader);
        glAttachShader(mshaderProgram, fragmentShader);

        bool linkSuccessfull = LinkProgram(mshaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        if (linkSuccessfull) return { RESULT_OK };
        else return { ERROR_SHADER, "Unable to link shader program" };
    }

    Status OpenGLShader::LinkProgram(GLuint shaderProgram)
    {
        glLinkProgram(shaderProgram);

        GLint linkSuccess;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkSuccess);
        if (linkSuccess != GL_TRUE)
        {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            return { ERROR_SHADER, std::string("Unable to link shader program. ").append(infoLog) };
        }

        return { RESULT_OK };
    };


    GLuint OpenGLShader::CompileShader(const std::string& shaderSource, ShaderType type) const
    {
        GLuint shader = glCreateShader(MapShaderType(type));

        // Shader creation failed
        if (shader == 0) return shader;

        const char* src = shaderSource.c_str();
        glShaderSource(shader, 1, &src, nullptr);

        glCompileShader(shader);

        if (!HasShaderCompiledProperly(shader))
        {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);

            // TODO: Errors should not be logged to the terminal
            std::cout << infoLog << '\n';

            return 0;
        }

        return shader;
    }

    bool OpenGLShader::HasShaderCompiledProperly(GLuint shader) const
    {
        GLint shaderCompileStatus;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompileStatus);

        if (shaderCompileStatus == GL_TRUE) return true;
        return false;
    }

    GLenum OpenGLShader::MapShaderType(ShaderType type) const
    {
        if (type == ShaderType::VERTEX_SHADER) return GL_VERTEX_SHADER;
        return GL_FRAGMENT_SHADER;
    }

    GLint OpenGLShader::GetUniformLocation(const char* name) const
    {
        return glGetUniformLocation(mshaderProgram, name);
    }

}
