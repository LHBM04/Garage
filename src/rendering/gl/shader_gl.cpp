#include "shader_gl.hpp"

#include <fstream>
#include <string>

#include <glad/glad.h>

namespace Garage
{
	void ShaderGL::SetUniformInt(Name name, int value)
	{
		const GLint location = glGetUniformLocation(handle, name.data());
		glUniform1i(location, value);
	}

	void ShaderGL::SetUniformFloat(Name name, float value)
	{
		const GLint location = glGetUniformLocation(handle, name.data());
		glUniform1f(location, value);
	}

	void ShaderGL::SetUniformVector2(Name name, void* value)
	{
		const GLint location = glGetUniformLocation(handle, name.data());
		glUniform2fv(location, 1, static_cast<const float*>(value));
	}

	void ShaderGL::SetUniformVector3(Name name, void* value)
	{
		const GLint location = glGetUniformLocation(handle, name.data());
		glUniform3fv(location, 1, static_cast<const float*>(value));
	}

	void ShaderGL::SetUniformVector4(Name name, void* value)
	{
		const GLint location = glGetUniformLocation(handle, name.data());
		glUniform4fv(location, 1, static_cast<const float*>(value));
	}

	void ShaderGL::SetUniformMatrix4x4(Name name, void* value)
	{
		const GLint location = glGetUniformLocation(handle, name.data());
		glUniformMatrix4fv(location, 1, GL_FALSE, static_cast<const float*>(value));
	}

	bool ShaderGL::Load(Path& path)
	{
		std::ifstream vertexShaderFile(path.string() + ".vert");
		if (!vertexShaderFile.is_open())
		{
			return false;
		}

		const std::string vertexShaderSource{ std::istreambuf_iterator<char>(vertexShaderFile),
											  std::istreambuf_iterator<char>() };
		const UInt32      vertexShader{ Compile(EType::Vertex, vertexShaderSource) };
		if (vertexShader == 0)
		{
			return false;
		}

		std::ifstream fragmentShaderFile(path.string() + ".frag");
		if (!fragmentShaderFile.is_open())
		{
			return false;
		}

		const std::string fragmentShaderSource{ std::istreambuf_iterator<char>(fragmentShaderFile),
											    std::istreambuf_iterator<char>() };
		const UInt32      fragmentShader{ Compile(EType::Vertex, vertexShaderSource) };
		if (vertexShader == 0)
		{
			return false;
		}

		handle = glCreateProgram();
		glAttachShader(handle, vertexShader);
		glAttachShader(handle, fragmentShader);
		glLinkProgram(handle);

		GLint isLinked{ 0 };
        glGetProgramiv(handle, GL_LINK_STATUS, &isLinked);
		if (!isLinked)
		{
            glDeleteProgram(handle);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
        }

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return true;
	}

	UInt32 ShaderGL::Compile(EType type, Source source)
	{
		GLuint shader{ 0 };
		switch (type)
		{
			case EType::Vertex:
				shader = glCreateShader(GL_VERTEX_SHADER);
				break;
			case EType::Fragment:
				shader = glCreateShader(GL_FRAGMENT_SHADER);
				break;
			default:
				return 0;
		}

		const GLchar* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		GLint isCompiled{ 0 };
        glGetProgramiv(handle, GL_LINK_STATUS, &isCompiled);
        if (!isCompiled)
        {
            glDeleteShader(shader);
            return 0;
        }
		
		return shader;
	}

	void ShaderGL::Use()
	{
		if (handle != 0)
		{
			glUseProgram(handle);
		}
	}
}