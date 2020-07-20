#include "../include/Shader.hpp"

#include <iostream>

Shader::Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
:	m_vertexShaderFilePath(vertexShaderFilePath),
	m_fragmentShaderFilePath(fragmentShaderFilePath)
{ /* */ }

Shader::Shader(const ShaderSources& source)
:	m_vertexShaderFilePath(""),
	m_fragmentShaderFilePath("")
{ /* */ }
