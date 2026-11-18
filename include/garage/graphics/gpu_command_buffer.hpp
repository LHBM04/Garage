#ifndef GARAGE_GRAPHICS_GPU_COMMAND_BUFFER_HPP
#define GARAGE_GRAPHICS_GPU_COMMAND_BUFFER_HPP

#include <span>
#include <string>
#include <type_traits>

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "../math/color_rgba.hpp"

#include "gpu_types.hpp"

namespace Garage
{
    class GPUTarget;
    class IGPUBuffer;
    class IGPUDevice;
    class IGPUFence;
    class IGPUGraphicsPipelineState;
    class IGPUTexture2D;

    class GARAGE_API IGPUCommandBuffer
    {
    public:
        static constexpr UInt32 TEXTURE_SLOT_COUNT  = 8;
        static constexpr UInt32 ROOT_CONSTANT_SLOT  = 3;
        static constexpr UInt32 ROOT_CONSTANT_COUNT = 4;

        IGPUCommandBuffer() noexcept          = default;
        virtual ~IGPUCommandBuffer() noexcept = default;

        IGPUCommandBuffer(const IGPUCommandBuffer&)            = delete;
        IGPUCommandBuffer& operator=(const IGPUCommandBuffer&) = delete;

        IGPUCommandBuffer(IGPUCommandBuffer&&)            = delete;
        IGPUCommandBuffer& operator=(IGPUCommandBuffer&&) = delete;

        [[nodiscard]] virtual Result<void, std::pmr::string> Reset()                            = 0;
        [[nodiscard]] virtual Result<void, std::pmr::string> Submit(IGPUFence* fence = nullptr) = 0;

        template <class T>
        [[nodiscard]] Result<void, std::pmr::string> SetGraphicsConstants(UInt32 rootSlot, const T& constants);

        template <class T>
        [[nodiscard]] Result<void, std::pmr::string> SetGraphicsConstants(const T& constants);

        [[nodiscard]] virtual Result<void, std::pmr::string> SetGraphicsConstants(UInt32                rootSlot,
                                                                                  std::span<const Byte> bytes) = 0;

        virtual void
                SetGraphicsRootConstants(UInt32 rootSlot, std::span<const Byte> bytes, UInt32 offset = 0) noexcept = 0;
        virtual void SetGPUGraphicsPipelineState(const IGPUGraphicsPipelineState& pipelineState) noexcept          = 0;

        [[nodiscard]] virtual Result<void, std::pmr::string>
                BindTextures(std::span<const IGPUTexture2D* const, TEXTURE_SLOT_COUNT> textures) = 0;

        virtual void CopyGPUBuffer(const IGPUBuffer& destination,
                                   UInt64            destinationOffset,
                                   const IGPUBuffer& source,
                                   UInt64            sourceOffset,
                                   UInt64            sizeInBytes) noexcept = 0;

        virtual void SetPrimitiveTopology(EGPUPrimitiveTopology primitive) noexcept = 0;

        virtual void SetViewport(const GPUViewport& viewport) noexcept   = 0;
        virtual void SetScissorRect(const GPURect& scissorRect) noexcept = 0;

        virtual void SetVertexBuffer(UInt32 slot, const GPUVertexBufferView& view) noexcept = 0;
        virtual void SetIndexBuffer(const GPUIndexBufferView& view) noexcept                = 0;

        virtual void Draw(UInt32 vertexCount, UInt32 firstVertex = 0) noexcept                            = 0;
        virtual void DrawIndexed(UInt32 indexCount, UInt32 firstIndex = 0, Int32 baseVertex = 0) noexcept = 0;
        virtual void DrawInstanced(UInt32 vertexCountPerInstance,
                                   UInt32 instanceCount,
                                   UInt32 firstVertex   = 0,
                                   UInt32 firstInstance = 0) noexcept                                     = 0;
        virtual void DrawIndexedInstanced(UInt32 indexCountPerInstance,
                                          UInt32 instanceCount,
                                          UInt32 firstIndex    = 0,
                                          Int32  baseVertex    = 0,
                                          UInt32 firstInstance = 0) noexcept                              = 0;

        virtual void ClearRenderTarget(const ColorRGBAFloat& color) noexcept = 0;
        virtual void ClearDepthStencil(Float32 depth) noexcept               = 0;
        virtual void SetGPUTarget(const GPUTarget& target) noexcept          = 0;
        virtual void EndRenderPass() noexcept                                = 0;

        [[nodiscard]] virtual const IGPUDevice*  GetDevice() const noexcept             = 0;
        [[nodiscard]] virtual EGPUTextureFormat  GetActiveTargetFormat() const noexcept = 0;
        [[nodiscard]] virtual const GPURect&     GetActiveTargetBounds() const noexcept = 0;
        [[nodiscard]] virtual const GPUViewport& GetActiveViewport() const noexcept     = 0;
    };

    template <class T>
    inline Result<void, std::pmr::string> IGPUCommandBuffer::SetGraphicsConstants(UInt32 rootSlot, const T& constants)
    {
        static_assert(std::is_trivially_copyable_v<T>, "상수 버퍼 타입은 trivially copyable이어야 합니다.");
        return SetGraphicsConstants(rootSlot, std::as_bytes(std::span(&constants, 1)));
    }

    template <class T>
    inline Result<void, std::pmr::string> IGPUCommandBuffer::SetGraphicsConstants(const T& constants)
    {
        return SetGraphicsConstants(T::ROOT_SLOT, constants);
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_COMMAND_BUFFER_HPP
