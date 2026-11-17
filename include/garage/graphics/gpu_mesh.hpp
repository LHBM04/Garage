#ifndef GARAGE_GRAPHICS_GPU_MESH_HPP
#define GARAGE_GRAPHICS_GPU_MESH_HPP

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "gpu_types.hpp"

namespace Garage
{
    class GPUBuffer;

    struct GARAGE_API GPUMeshCreateInfo final
    {
        const GPUBuffer*      vertexBuffer     = nullptr;
        GPUVertexBufferView   vertexBufferView = {};
        const GPUBuffer*      indexBuffer      = nullptr;
        GPUIndexBufferView    indexBufferView  = {};
        EGPUPrimitiveTopology primitive        = EGPUPrimitiveTopology::TRIANGLE_LIST;
    };

    class GARAGE_API GPUMesh final
    {
    public:
        explicit GPUMesh(const GPUMeshCreateInfo& createInfo) noexcept;
        ~GPUMesh() noexcept = default;

        GPUMesh(const GPUMesh&)            = delete;
        GPUMesh& operator=(const GPUMesh&) = delete;

        GPUMesh(GPUMesh&&) noexcept            = default;
        GPUMesh& operator=(GPUMesh&&) noexcept = default;

        [[nodiscard]] const GPUBuffer*           GetVertexBuffer() const noexcept;
        [[nodiscard]] UInt32                     GetVertexCount() const noexcept;
        [[nodiscard]] const GPUVertexBufferView& GetVertexBufferView() const noexcept;

        [[nodiscard]] const GPUBuffer*          GetIndexBuffer() const noexcept;
        [[nodiscard]] UInt32                    GetIndexCount() const noexcept;
        [[nodiscard]] const GPUIndexBufferView& GetIndexBufferView() const noexcept;

        [[nodiscard]] EGPUPrimitiveTopology GetTopology() const noexcept;

    private:
        const GPUBuffer*      vertexBuffer     = nullptr;
        GPUVertexBufferView   vertexBufferView = {};
        const GPUBuffer*      indexBuffer      = nullptr;
        GPUIndexBufferView    indexBufferView  = {};
        EGPUPrimitiveTopology primitive        = EGPUPrimitiveTopology::TRIANGLE_LIST;
    };

    inline GPUMesh::GPUMesh(const GPUMeshCreateInfo& createInfo) noexcept
        : vertexBuffer(createInfo.vertexBuffer)
        , vertexBufferView(createInfo.vertexBufferView)
        , indexBuffer(createInfo.indexBuffer)
        , indexBufferView(createInfo.indexBufferView)
        , primitive(createInfo.primitive)
    {
        ASSERT(this->vertexBuffer != nullptr);
        ASSERT(this->indexBuffer != nullptr);
    }

    inline const GPUBuffer* GPUMesh::GetVertexBuffer() const noexcept
    {
        return vertexBuffer;
    }

    inline const GPUVertexBufferView& GPUMesh::GetVertexBufferView() const noexcept
    {
        return vertexBufferView;
    }

    inline const GPUBuffer* GPUMesh::GetIndexBuffer() const noexcept
    {
        return indexBuffer;
    }

    inline const GPUIndexBufferView& GPUMesh::GetIndexBufferView() const noexcept
    {
        return indexBufferView;
    }

    inline EGPUPrimitiveTopology GPUMesh::GetTopology() const noexcept
    {
        return primitive;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_MESH_HPP
