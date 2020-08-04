#include "../include/Shader.hpp"

#include <iostream>

Shader::Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
:	mvertexShaderFilePath(vertexShaderFilePath),
	mfragmentShaderFilePath(fragmentShaderFilePath)
{ /* */ }

Shader::Shader(const ShaderSources& source)
:	mvertexShaderFilePath(""),
	mfragmentShaderFilePath("")
{ /* */ }
