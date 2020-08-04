#include "../include/OpenGLShader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

OpenGLShader::OpenGLShader(
	const std::string& vertexShaderFilePath, 
	const std::string& fragmentShaderFilePath)
	:	Shader(vertexShaderFilePath, fragmentShaderFilePath),
		mcompilationSource(ShaderSourceType::SHADER_FILES),
		mshaderProgram(0)
{ }

OpenGLShader::OpenGLShader(const ShaderSources& source)
:	Shader(source),
	mcompilationSource(ShaderSourceType::SHADER_SOURCES),
	mshaderProgram(0)
{}


bool OpenGLShader::Init()
{
	if(mcompilationSource == ShaderSourceType::SHADER_FILES) 
	{
		msources = ExtractShaderSources();

		if(msources.vertexShader   == "") return false;
		if(msources.fragmentShader == "") return false;
	}

	return CompileShaders();
}

void OpenGLShader::Cleanup()
{
	glDeleteProgram(mshaderProgram);
}

bool OpenGLShader::SetMat4( const char* name, const void* data)
{
	GLint uniformLocation = GetUniformLocation(name);
	if(uniformLocation == -1) return false;

	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, (float *)data);
	return true;
}

bool OpenGLShader::SetVec4( const char* name, const Vec4& data)
{
	GLint uniformLocation = GetUniformLocation(name);
	if(uniformLocation == -1) return false;
	glUniform4fv(uniformLocation, 1, (float *)(&data));
	return true;
}
bool OpenGLShader::SetVec3( const char* name, const Vec3& data)
{
	GLint uniformLocation = GetUniformLocation(name);
	if(uniformLocation == -1) return false;
	glUniform3fv(uniformLocation, 1, (float *)(&data));
	return true;
}

bool OpenGLShader::SetVec2( const char* name, const Vec2& data)
{
	GLint uniformLocation = GetUniformLocation(name);
	if(uniformLocation == -1) return false;
	glUniform2fv(uniformLocation, 1, (float *)(&data));
	return true;
}

bool OpenGLShader::SetInt(const char* name, const int& data)
{
	GLint uniformLocation = GetUniformLocation(name);
	if(uniformLocation == -1) return false;
	glUniform1i(uniformLocation, data);
	return true;
}

bool OpenGLShader::SetFloat(const char* name, const float& data)
{
	GLint uniformLocation = GetUniformLocation(name);
	if(uniformLocation == -1) return false;
	glUniform1f(uniformLocation, data);
	return true;
}

ShaderSources OpenGLShader::ExtractShaderSources() const
{
	std::fstream vsFileStream; 
	std::fstream fsFileStream; 

	vsFileStream.open(mvertexShaderFilePath,   std::fstream::in);
	fsFileStream.open(mfragmentShaderFilePath, std::fstream::in);

	if(!vsFileStream) return { "", "" };
	if(!fsFileStream) return { "", "" };

	std::stringstream vsStreamSource;
	std::stringstream fsStreamSource;
	
	vsStreamSource << vsFileStream.rdbuf();
	fsStreamSource << fsFileStream.rdbuf();

	vsFileStream.close();
	fsFileStream.close();

	return { vsStreamSource.str(), fsStreamSource.str() };
}


bool OpenGLShader::CompileShaders() 
{
	GLuint vertexShader   = CompileShader(msources.vertexShader,   ShaderType::VERTEX_SHADER  );
	if(vertexShader == 0) return false;

	GLuint fragmentShader = CompileShader(msources.fragmentShader, ShaderType::FRAGMENT_SHADER);
	if(fragmentShader == 0)
	{
		glDeleteShader(vertexShader);
		return false;
	}

	mshaderProgram = glCreateProgram();
	if(mshaderProgram == 0) return false;
	
	glAttachShader(mshaderProgram, vertexShader);
	glAttachShader(mshaderProgram, fragmentShader);

	bool linkSuccessfull = LinkProgram(mshaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return linkSuccessfull;
}

bool OpenGLShader::LinkProgram(GLuint shaderProgram)
{
	glLinkProgram(shaderProgram);

	GLint linkSuccess;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkSuccess);
	if(linkSuccess != GL_TRUE)
	{
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cout << infoLog << '\n';
		return false;
	}

	return true;
};


GLuint OpenGLShader::CompileShader(const std::string& shaderSource, ShaderType type) const
{
	GLuint shader = glCreateShader(MapShaderType(type));
	
	// Shader creation failed
	if(shader == 0) return shader;

	const char* src = shaderSource.c_str(); 
	glShaderSource(shader, 1, &src, nullptr);

	glCompileShader(shader);

	if(!HasShaderCompiledProperly(shader))
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

	if(shaderCompileStatus == GL_TRUE) return true;
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
