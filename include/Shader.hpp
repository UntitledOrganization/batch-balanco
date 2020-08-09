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

			virtual Status SetMat4( const char* name, const void* data)  = 0;
			virtual Status SetVec4( const char* name, const Vec4& data)  = 0;
			virtual Status SetVec3( const char* name, const Vec3& data)  = 0;
			virtual Status SetVec2( const char* name, const Vec2& data)  = 0;

			virtual Status SetInt(const char* name, const int& data)     = 0;
			virtual Status SetFloat(const char* name, const float& data) = 0;

		protected: 
			enum class ShaderType
			{
				VERTEX_SHADER,
				FRAGMENT_SHADER
			};

			std::string mVertexShaderFilePath;
			std::string mFragmentShaderFilePath;

			ShaderSources mShaderSource;
	};

}
