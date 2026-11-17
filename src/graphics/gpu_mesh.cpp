#include "garage/graphics/gpu_mesh.hpp"

#include "garage/core/types.hpp"

#include "garage/graphics/gpu_types.hpp"

namespace Garage
{
    UInt32 GPUMesh::GetVertexCount() const noexcept
    {
        if (vertexBufferView.strideInBytes == 0)
        {
            return 0;
        }

        return vertexBufferView.sizeInBytes / vertexBufferView.strideInBytes;
    }

    UInt32 GPUMesh::GetIndexCount() const noexcept
    {
        const UInt32 INDEX_SIZE = indexBufferView.format == EGPUIndexFormat::UINT16 ? 2u : 4u;
        return indexBufferView.sizeInBytes / INDEX_SIZE;
    }
} // namespace Garage
