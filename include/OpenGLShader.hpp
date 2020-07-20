#pragma once
#include "Shader.hpp"

#include <GL/glew.h>

class OpenGLShader : public Shader {
public:
	OpenGLShader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
	OpenGLShader(const ShaderSources& source);
	OpenGLShader(const OpenGLShader& s) = delete;

	// Compiles shader and create shader program.
	// Returns true if both succeed, false otherwise.
	virtual bool Init()    override;
	virtual void Cleanup() override;

	inline virtual void Bind()   override { glUseProgram(m_shaderProgram);  };
	inline virtual void Unbind() override { glUseProgram(0); 				};

	virtual bool SetMat4( const char* name, const void* data) override;
	virtual bool SetVec4( const char* name, const Vec4& data) override;
	virtual bool SetVec3( const char* name, const Vec3& data) override;
	virtual bool SetVec2( const char* name, const Vec2& data) override;

	virtual bool SetInt(const char* name, const int& data)     override;
	virtual bool SetFloat(const char* name, const float& data) override;

private:
	enum class ShaderSourceType
	{
		SHADER_FILES,
		SHADER_SOURCES,
		SHADER_SINGLE_SOURCE
	};

	ShaderSourceType m_compilationSource;
	ShaderSources m_sources;

	GLuint m_shaderProgram;	

	// Returns shader if compiled successfully,
	// 0 otherwise.
	GLuint CompileShader(const std::string& shaderSource, ShaderType type) const;

	bool CompileShaders();
	bool LinkProgram();
	bool LinkProgram(GLuint shaderProgram);
	bool HasShaderCompiledProperly(GLuint shader) const;
	ShaderSources ExtractShaderSources() const;

	GLenum MapShaderType(ShaderType type) const;
	GLint GetUniformLocation(const char* name) const;
};
