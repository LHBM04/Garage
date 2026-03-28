#include "rendering/gl/render_device_gl.hpp"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

namespace Garage
{
	RenderDeviceGL::~RenderDeviceGL()
	{
        Release();
	}

	bool RenderDeviceGL::Initialize(const RenderDevice::Specification& specification)
	{
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			return false;
		}
		glViewport(0, 0, specification.width, specification.height);
        
		glEnable(GL_DEPTH_TEST);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

		return true;
    }

	void RenderDeviceGL::Release()
	{
		
    }

	void RenderDeviceGL::Clear()
	{
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RenderDeviceGL::Present()
	{

	}
}