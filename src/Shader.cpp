#include "../include/Shader.hpp"

#include <iostream>

namespace sbb
{

	Shader::Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
		: mVertexShaderFilePath(vertexShaderFilePath),
		mFragmentShaderFilePath(fragmentShaderFilePath)
	{ /* */
	}

	Shader::Shader(const ShaderSources& source)
		:	mVertexShaderFilePath(""),
		mFragmentShaderFilePath("")
	{ /* */
	}

}
