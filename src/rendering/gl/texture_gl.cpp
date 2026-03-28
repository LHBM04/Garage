#include "texture_gl.hpp"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Garage
{
	TextureGL::~TextureGL()
	{
        glDeleteTextures(1, &handle);
    }

	bool TextureGL::Load(Path& path)
    {
        stbi_set_flip_vertically_on_load(true);

        UInt8* data = stbi_load(path.string().c_str(), &width, &height, nullptr, 4);
        if (!data)
        {
            return false;
        }

        GLenum format{ 0 };
        if (stbi_is_hdr(path.string().c_str()))
        {
            format = GL_RGBA16F;
        }
        else
        {
            format = GL_RGBA8;
        }

        glGenTextures(1, &handle);
        glBindTexture(GL_TEXTURE_2D, handle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
	}

	void TextureGL::Bind() const
	{
        if (handle != 0)
        {
            glBindTexture(GL_TEXTURE_2D, handle);
        }
    }

	void TextureGL::Unbind() const
	{
        if (handle != 0)
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}