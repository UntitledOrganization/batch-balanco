#pragma once

#include <string>

#include "Types.hpp"
#include "Result.hpp"

namespace sbb
{

	struct ShaderSources {
		std::string vertexShader;
		std::string fragmentShader;
	};

	class Shader {
		public:
			Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
			Shader(const ShaderSources& source);
			Shader(const Shader& s) = delete;

			// Accepts both vertex and fragment shaders in one file
			Shader(const std::string& oneFileShaders) {};

			virtual Status Init()    = 0;

			virtual void Cleanup() = 0;
			virtual void Bind()    = 0;
			virtual void Unbind()  = 0;

			virtual bool SetMat4( const char* name, const void* data)  = 0;
			virtual bool SetVec4( const char* name, const Vec4& data)  = 0;
			virtual bool SetVec3( const char* name, const Vec3& data)  = 0;
			virtual bool SetVec2( const char* name, const Vec2& data)  = 0;

			virtual bool SetInt(const char* name, const int& data)     = 0;
			virtual bool SetFloat(const char* name, const float& data) = 0;

		protected: 
			enum class ShaderType
			{
				VERTEX_SHADER,
				FRAGMENT_SHADER
			};

			std::string mvertexShaderFilePath;
			std::string mfragmentShaderFilePath;

			ShaderSources mshaderSource;
	};

}
