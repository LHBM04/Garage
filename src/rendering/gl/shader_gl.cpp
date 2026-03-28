#include "rendering/gl/shader_gl.hpp"

namespace Garage
{
	Shader::Handle ShaderGL::GetHandle() const
	{
		return 0;
	}

	void ShaderGL::SetUniformInt(Name name, int value)
	{
		
	}

	void ShaderGL::SetUniformFloat(Name name, float value)
	{
		
	}

	void ShaderGL::SetUniformVector2(Name name, void* value)
	{
		
	}

	void ShaderGL::SetUniformVector3(Name name, void* value)
	{
		
	}

	void ShaderGL::SetUniformVector4(Name name, void* value)
	{
		
	}

    void ShaderGL::SetUniformMatrix4x4(Name name, void* value)
    {
	}

    void ShaderGL::Compile(const std::string& vertexSource, const std::string& fragmentSource)
    {
	}

    void ShaderGL::Use()
    {
	}
}