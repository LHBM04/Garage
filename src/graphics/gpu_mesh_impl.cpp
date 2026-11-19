#include "gpu_mesh_impl.hpp"

#include "garage/core/common.hpp"

#include "garage/graphics/gpu_buffer.hpp"
#include "garage/graphics/gpu_types.hpp"

namespace Garage
{
    GPUMeshImpl::GPUMeshImpl(const IGPUBuffer*     vertexBuffer,
                             UInt32                vertexStride,
                             const IGPUBuffer*     indexBuffer,
                             EGPUIndexFormat       indexFormat,
                             EGPUPrimitiveTopology primitive) noexcept
        : topology(primitive)
    {
        ASSERT(vertexBuffer != nullptr);
        ASSERT(vertexStride > 0);

        const UInt32 VERTEX_BUFFER_SIZE = static_cast<UInt32>(vertexBuffer->GetSizeInBytes());
        vertexBufferView.buffer         = vertexBuffer;
        vertexBufferView.offset         = 0;
        vertexBufferView.sizeInBytes    = VERTEX_BUFFER_SIZE;
        vertexBufferView.strideInBytes  = vertexStride;
        vertexCount                     = VERTEX_BUFFER_SIZE / vertexStride;

        if (indexBuffer != nullptr)
        {
            const UInt32 INDEX_BUFFER_SIZE = static_cast<UInt32>(indexBuffer->GetSizeInBytes());
            const UInt32 INDEX_STRIDE      = (indexFormat == EGPUIndexFormat::UINT16) ? 2u : 4u;

            indexBufferView.buffer      = indexBuffer;
            indexBufferView.offset      = 0;
            indexBufferView.sizeInBytes = INDEX_BUFFER_SIZE;
            indexBufferView.format      = indexFormat;
            indexCount                  = INDEX_BUFFER_SIZE / INDEX_STRIDE;
        }
    }
} // namespace Garage
