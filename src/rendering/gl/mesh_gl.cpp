#include "mesh_gl.hpp"

#include <glad/glad.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Garage
{
	MeshGL::~MeshGL()
	{
        if (vertexArray)
		{
            glDeleteVertexArrays(1, &vertexArray);
            vertexArray = 0;
        }

		if (vertexBuffer)
		{
			glDeleteBuffers(1, &vertexBuffer);
			vertexBuffer = 0;
		}

		if (elementBuffer)
		{
			glDeleteBuffers(1, &elementBuffer);
			elementBuffer = 0;
        }
	}

	bool MeshGL::Load(Path& path)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!scene)
		{
			return false;
		}

		for (Size index = 0; index < scene->mNumMeshes; ++index)
		{
			const aiMesh* mesh = scene->mMeshes[index];

			for (Size vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
			{
				Vertex vertex;

				vertex.position[0] = mesh->mVertices[vertexIndex].x;
				vertex.position[1] = mesh->mVertices[vertexIndex].y;
				vertex.position[2] = mesh->mVertices[vertexIndex].z;
				
				if (mesh->HasNormals())
				{
					vertex.normal[0] = mesh->mNormals[vertexIndex].x;
					vertex.normal[1] = mesh->mNormals[vertexIndex].y;
					vertex.normal[2] = mesh->mNormals[vertexIndex].z;
				}
				else
				{
					vertex.normal[0] = 0.0f;
					vertex.normal[1] = 0.0f;
					vertex.normal[2] = 0.0f;
				}

				if (mesh->HasTextureCoords(0))
				{
					vertex.texCoords[0] = mesh->mTextureCoords[0][vertexIndex].x;
					vertex.texCoords[1] = mesh->mTextureCoords[0][vertexIndex].y;
				}
				else
				{
					vertex.texCoords[0] = 0.0f;
					vertex.texCoords[1] = 0.0f;
				}
				
				vertices.push_back(vertex);
			}

			for (Size faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
			{
				const aiFace& face = mesh->mFaces[faceIndex];
                for (Size index = 0; index < face.mNumIndices; ++index)
				{
					indices.push_back(face.mIndices[index]);
				}
			}
		}

		Bind();

		return true;
    }

	void MeshGL::Bind()
	{
        glGenVertexArrays(1, (GLuint*)&vertexArray);
		glGenBuffers(1, &vertexBuffer);
		glGenBuffers(1, &elementBuffer);

		glBindVertexArray(vertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoords)));

		glBindVertexArray(0);
	}

	void MeshGL::Unbind() const
	{
		glBindVertexArray(0);
	}

	void MeshGL::Draw() const
	{
		if (vertexArray != 0)
		{
			glBindVertexArray(vertexArray);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
			glBindVertexArray(0);
		}
	}
}