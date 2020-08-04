#include "../include/Shader.hpp"

#include <iostream>

namespace sbb
{

	Shader::Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
		:	mvertexShaderFilePath(vertexShaderFilePath),
		mfragmentShaderFilePath(fragmentShaderFilePath)
	{ /* */ }

	Shader::Shader(const ShaderSources& source)
		:	mvertexShaderFilePath(""),
		mfragmentShaderFilePath("")
	{ /* */ }

}
