#ifndef GARAGE_GRAPHICS_GPU_MESH_HPP
#define GARAGE_GRAPHICS_GPU_MESH_HPP

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "gpu_types.hpp"

namespace Garage
{
    class IGPUBuffer;

    class GARAGE_API IGPUMesh
    {
    public:
        IGPUMesh() noexcept          = default;
        virtual ~IGPUMesh() noexcept = default;

        IGPUMesh(const IGPUMesh&)            = delete;
        IGPUMesh& operator=(const IGPUMesh&) = delete;

        IGPUMesh(IGPUMesh&&)            = delete;
        IGPUMesh& operator=(IGPUMesh&&) = delete;

        [[nodiscard]] virtual const IGPUBuffer*          GetVertexBuffer() const noexcept     = 0;
        [[nodiscard]] virtual UInt32                     GetVertexCount() const noexcept      = 0;
        [[nodiscard]] virtual const GPUVertexBufferView& GetVertexBufferView() const noexcept = 0;

        [[nodiscard]] virtual const IGPUBuffer*         GetIndexBuffer() const noexcept     = 0;
        [[nodiscard]] virtual UInt32                    GetIndexCount() const noexcept      = 0;
        [[nodiscard]] virtual const GPUIndexBufferView& GetIndexBufferView() const noexcept = 0;

        [[nodiscard]] virtual EGPUPrimitiveTopology GetTopology() const noexcept = 0;
    };
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_MESH_HPP
