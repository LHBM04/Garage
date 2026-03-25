#include "Renderer.hpp"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

namespace Garage
{
	bool Renderer::Initialize(const Specification& specification) noexcept
	{
        int version = gladLoadGL();
		if (version == 0)
		{
			return false;
		}

		glViewport(specification.x, specification.y, 
				   specification.width, specification.height);
        
		return true;
    }

    void Renderer::Clear() noexcept
    {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}