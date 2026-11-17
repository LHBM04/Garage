#ifndef GARAGE_GRAPHICS_GPU_COMMAND_BUFFER_HPP
#define GARAGE_GRAPHICS_GPU_COMMAND_BUFFER_HPP

#include <array>
#include <span>
#include <type_traits>

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "../math/color_rgba.hpp"

#include "gpu_types.hpp"

namespace Garage
{
    class GPUBuffer;
    class GPUDevice;
    class GPUFence;
    class GPUGraphicsPipelineState;
    class GPUTarget;
    class GPUTexture2D;

    struct GARAGE_API GPUCommandBufferCreateInfo final
    {
        const GPUDevice* device = nullptr;
    };

    class GARAGE_API GPUCommandBuffer final
    {
    public:
        static constexpr UInt32 TEXTURE_SLOT_COUNT  = 8;
        static constexpr UInt32 ROOT_CONSTANT_SLOT  = 3;
        static constexpr UInt32 ROOT_CONSTANT_COUNT = 4;

        explicit GPUCommandBuffer(const GPUCommandBufferCreateInfo& createInfo) noexcept;
        ~GPUCommandBuffer() noexcept;

        GPUCommandBuffer(const GPUCommandBuffer&)            = delete;
        GPUCommandBuffer& operator=(const GPUCommandBuffer&) = delete;

        GPUCommandBuffer(GPUCommandBuffer&& other) noexcept;
        GPUCommandBuffer& operator=(GPUCommandBuffer&& other) noexcept;

        [[nodiscard]] bool Reset() noexcept;
        [[nodiscard]] bool Submit(GPUFence* fence = nullptr) noexcept;

        template <class T>
        [[nodiscard]] bool SetGraphicsConstants(UInt32 rootSlot, const T& constants) noexcept;

        template <class T>
        [[nodiscard]] bool SetGraphicsConstants(const T& constants) noexcept;

        [[nodiscard]] bool SetGraphicsConstants(UInt32 rootSlot, std::span<const Byte> bytes) noexcept;

        void SetGraphicsRootConstants(UInt32 rootSlot, std::span<const Byte> bytes, UInt32 offset = 0) noexcept;
        void SetGPUGraphicsPipelineState(const GPUGraphicsPipelineState& pipelineState) noexcept;

        [[nodiscard]] bool BindTextures(std::span<const GPUTexture2D* const, TEXTURE_SLOT_COUNT> textures) noexcept;

        void CopyGPUBuffer(const GPUBuffer& destination,
                           UInt64           destinationOffset,
                           const GPUBuffer& source,
                           UInt64           sourceOffset,
                           UInt64           sizeInBytes) noexcept;

        void SetPrimitiveTopology(EGPUPrimitiveTopology primitive) noexcept;

        void SetViewport(const GPUViewport& viewport) noexcept;
        void SetScissorRect(const GPURect& scissorRect) noexcept;

        void SetVertexBuffer(UInt32 slot, const GPUVertexBufferView& view) noexcept;
        void SetIndexBuffer(const GPUIndexBufferView& view) noexcept;

        void Draw(UInt32 vertexCount, UInt32 firstVertex = 0) noexcept;
        void DrawIndexed(UInt32 indexCount, UInt32 firstIndex = 0, Int32 baseVertex = 0) noexcept;
        void DrawInstanced(UInt32 vertexCountPerInstance,
                           UInt32 instanceCount,
                           UInt32 firstVertex   = 0,
                           UInt32 firstInstance = 0) noexcept;
        void DrawIndexedInstanced(UInt32 indexCountPerInstance,
                                  UInt32 instanceCount,
                                  UInt32 firstIndex    = 0,
                                  Int32  baseVertex    = 0,
                                  UInt32 firstInstance = 0) noexcept;

        void ClearRenderTarget(const ColorRGBAFloat& color) noexcept;
        void ClearDepthStencil(Float32 depth) noexcept;
        void SetGPUTarget(const GPUTarget& target) noexcept;
        void EndRenderPass() noexcept;

        [[nodiscard]] EGPUTextureFormat  GetActiveTargetFormat() const noexcept;
        [[nodiscard]] const GPURect&     GetActiveTargetBounds() const noexcept;
        [[nodiscard]] const GPUViewport& GetActiveViewport() const noexcept;
        [[nodiscard]] void*              GetNativeCommandBuffer() const noexcept;
        [[nodiscard]] void*              GetActiveRenderPass() const noexcept;

    private:
        [[nodiscard]] bool EnsureRenderPass() noexcept;

        const GPUDevice* device              = nullptr;
        void*            commandBufferHandle = nullptr;
        void*            renderPassHandle    = nullptr;

        const GPUTarget*      activeTarget       = nullptr;
        EGPUTextureFormat     activeTargetFormat = EGPUTextureFormat::UNKNOWN;
        GPURect               activeTargetBounds = {};
        GPUViewport           activeViewport     = {};
        GPURect               activeScissor      = {};
        EGPUPrimitiveTopology activeTopology     = EGPUPrimitiveTopology::TRIANGLE_LIST;

        ColorRGBAFloat clearColor         = ColorRGBAFloat::GetBlack();
        Float32        clearDepth         = 1.0f;
        bool           shouldClearColor   = false;
        bool           shouldClearDepth   = false;
        bool           hasPendingViewport = false;
        bool           hasPendingScissor  = false;

        std::array<UInt32, ROOT_CONSTANT_COUNT> rootConstants = {};
    };

    inline EGPUTextureFormat GPUCommandBuffer::GetActiveTargetFormat() const noexcept
    {
        return activeTargetFormat;
    }

    inline const GPURect& GPUCommandBuffer::GetActiveTargetBounds() const noexcept
    {
        return activeTargetBounds;
    }

    inline const GPUViewport& GPUCommandBuffer::GetActiveViewport() const noexcept
    {
        return activeViewport;
    }

    inline void* GPUCommandBuffer::GetNativeCommandBuffer() const noexcept
    {
        return commandBufferHandle;
    }

    inline void* GPUCommandBuffer::GetActiveRenderPass() const noexcept
    {
        return renderPassHandle;
    }

    template <class T>
    inline bool GPUCommandBuffer::SetGraphicsConstants(UInt32 rootSlot, const T& constants) noexcept
    {
        static_assert(std::is_trivially_copyable_v<T>, "상수 버퍼 타입은 trivially copyable이어야 합니다.");
        return SetGraphicsConstants(rootSlot, std::as_bytes(std::span(&constants, 1)));
    }

    template <class T>
    inline bool GPUCommandBuffer::SetGraphicsConstants(const T& constants) noexcept
    {
        return SetGraphicsConstants(T::ROOT_SLOT, constants);
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_COMMAND_BUFFER_HPP
