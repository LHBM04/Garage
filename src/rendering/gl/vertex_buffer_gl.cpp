#include "vertex_buffer_gl.hpp"

#include <glad/glad.h>

namespace Garage
{
	VertexBufferGL::VertexBufferGL(const void* data, UInt32 size)
	{
        glCreateBuffers(1, &rendererID);
        glNamedBufferData(rendererID, size, data, GL_STATIC_DRAW);
    }

	VertexBufferGL::~VertexBufferGL()
	{
		glDeleteBuffers(1, &rendererID);
    }

	void VertexBufferGL::Bind() const
	{
        glBindBuffer(GL_ARRAY_BUFFER, rendererID);
	}

	void VertexBufferGL::Unbind() const
	{
        glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}