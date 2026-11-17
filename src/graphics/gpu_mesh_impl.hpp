#ifndef GARAGE_GRAPHICS_GPU_MESH_IMPL_HPP
#define GARAGE_GRAPHICS_GPU_MESH_IMPL_HPP

#include "garage/core/types.hpp"

#include "garage/graphics/gpu_mesh.hpp"
#include "garage/graphics/gpu_types.hpp"

namespace Garage
{
    class IGPUBuffer;

    class GPUMeshImpl final : public IGPUMesh
    {
    public:
        GPUMeshImpl(const IGPUBuffer*     vertexBuffer,
                    UInt32                vertexStride,
                    const IGPUBuffer*     indexBuffer,
                    EGPUIndexFormat       indexFormat,
                    EGPUPrimitiveTopology primitive) noexcept;
        ~GPUMeshImpl() noexcept override = default;

        void SetOwnedBuffers(IGPUBuffer* vertexBuffer, IGPUBuffer* indexBuffer) noexcept;

        [[nodiscard]] const IGPUBuffer*          GetVertexBuffer() const noexcept override;
        [[nodiscard]] UInt32                     GetVertexCount() const noexcept override;
        [[nodiscard]] const GPUVertexBufferView& GetVertexBufferView() const noexcept override;

        [[nodiscard]] const IGPUBuffer*         GetIndexBuffer() const noexcept override;
        [[nodiscard]] UInt32                    GetIndexCount() const noexcept override;
        [[nodiscard]] const GPUIndexBufferView& GetIndexBufferView() const noexcept override;

        [[nodiscard]] EGPUPrimitiveTopology GetTopology() const noexcept override;

        [[nodiscard]] bool        OwnsBuffers() const noexcept;
        [[nodiscard]] IGPUBuffer* GetOwnedVertexBuffer() const noexcept;
        [[nodiscard]] IGPUBuffer* GetOwnedIndexBuffer() const noexcept;

    private:
        IGPUBuffer*           ownedVertexBuffer = nullptr;
        IGPUBuffer*           ownedIndexBuffer  = nullptr;
        GPUVertexBufferView   vertexBufferView  = {};
        GPUIndexBufferView    indexBufferView   = {};
        UInt32                vertexCount       = 0;
        UInt32                indexCount        = 0;
        EGPUPrimitiveTopology topology          = EGPUPrimitiveTopology::TRIANGLE_LIST;
        bool                  ownsBuffers       = false;
    };

    inline void GPUMeshImpl::SetOwnedBuffers(IGPUBuffer* vertexBuffer, IGPUBuffer* indexBuffer) noexcept
    {
        ownedVertexBuffer = vertexBuffer;
        ownedIndexBuffer  = indexBuffer;
        ownsBuffers       = (vertexBuffer != nullptr || indexBuffer != nullptr);
    }

    inline const IGPUBuffer* GPUMeshImpl::GetVertexBuffer() const noexcept
    {
        return vertexBufferView.buffer;
    }

    inline UInt32 GPUMeshImpl::GetVertexCount() const noexcept
    {
        return vertexCount;
    }

    inline const GPUVertexBufferView& GPUMeshImpl::GetVertexBufferView() const noexcept
    {
        return vertexBufferView;
    }

    inline const IGPUBuffer* GPUMeshImpl::GetIndexBuffer() const noexcept
    {
        return indexBufferView.buffer;
    }

    inline UInt32 GPUMeshImpl::GetIndexCount() const noexcept
    {
        return indexCount;
    }

    inline const GPUIndexBufferView& GPUMeshImpl::GetIndexBufferView() const noexcept
    {
        return indexBufferView;
    }

    inline EGPUPrimitiveTopology GPUMeshImpl::GetTopology() const noexcept
    {
        return topology;
    }

    inline bool GPUMeshImpl::OwnsBuffers() const noexcept
    {
        return ownsBuffers;
    }

    inline IGPUBuffer* GPUMeshImpl::GetOwnedVertexBuffer() const noexcept
    {
        return ownedVertexBuffer;
    }

    inline IGPUBuffer* GPUMeshImpl::GetOwnedIndexBuffer() const noexcept
    {
        return ownedIndexBuffer;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_MESH_IMPL_HPP
