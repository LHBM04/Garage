#ifndef GARAGE_GRAPHICS_GPU_COMMAND_BUFFER_IMPL_HPP
#define GARAGE_GRAPHICS_GPU_COMMAND_BUFFER_IMPL_HPP

#include <array>
#include <span>
#include <string>

#include <SDL3/SDL_gpu.h>

#include "garage/core/types.hpp"

#include "garage/graphics/gpu_command_buffer.hpp"
#include "garage/graphics/gpu_target.hpp"
#include "garage/graphics/gpu_types.hpp"

#include "garage/math/color_rgba.hpp"

namespace Garage
{
    class GPUDeviceImpl;
    class GPUGraphicsPipelineStateImpl;
    class IGPUBuffer;
    class IGPUDevice;
    class IGPUFence;
    class IGPUGraphicsPipelineState;
    class IGPUTexture2D;

    class GPUCommandBufferImpl final : public IGPUCommandBuffer
    {
    public:
        explicit GPUCommandBufferImpl(const GPUDeviceImpl* device) noexcept;
        ~GPUCommandBufferImpl() noexcept override;

        [[nodiscard]] Result<void, std::pmr::string> Reset() override;
        [[nodiscard]] Result<void, std::pmr::string> Submit(IGPUFence* fence = nullptr) override;

        [[nodiscard]] Result<void, std::pmr::string> SetGraphicsConstants(UInt32                rootSlot,
                                                                          std::span<const Byte> bytes) override;

        void SetGraphicsRootConstants(UInt32                rootSlot,
                                      std::span<const Byte> bytes,
                                      UInt32                offset = 0) noexcept override;
        void SetGPUGraphicsPipelineState(const IGPUGraphicsPipelineState& pipelineState) noexcept override;

        [[nodiscard]] Result<void, std::pmr::string>
                BindTextures(std::span<const IGPUTexture2D* const, TEXTURE_SLOT_COUNT> textures) override;

        void CopyGPUBuffer(const IGPUBuffer& destination,
                           UInt64            destinationOffset,
                           const IGPUBuffer& source,
                           UInt64            sourceOffset,
                           UInt64            sizeInBytes) noexcept override;

        void SetPrimitiveTopology(EGPUPrimitiveTopology primitive) noexcept override;

        void SetViewport(const GPUViewport& viewport) noexcept override;
        void SetScissorRect(const GPURect& scissorRect) noexcept override;

        void SetVertexBuffer(UInt32 slot, const GPUVertexBufferView& view) noexcept override;
        void SetIndexBuffer(const GPUIndexBufferView& view) noexcept override;

        void Draw(UInt32 vertexCount, UInt32 firstVertex = 0) noexcept override;
        void DrawIndexed(UInt32 indexCount, UInt32 firstIndex = 0, Int32 baseVertex = 0) noexcept override;
        void DrawInstanced(UInt32 vertexCountPerInstance,
                           UInt32 instanceCount,
                           UInt32 firstVertex   = 0,
                           UInt32 firstInstance = 0) noexcept override;
        void DrawIndexedInstanced(UInt32 indexCountPerInstance,
                                  UInt32 instanceCount,
                                  UInt32 firstIndex    = 0,
                                  Int32  baseVertex    = 0,
                                  UInt32 firstInstance = 0) noexcept override;

        void ClearRenderTarget(const ColorRGBAFloat& color) noexcept override;
        void ClearDepthStencil(Float32 depth) noexcept override;
        void SetGPUTarget(const GPUTarget& target) noexcept override;
        void EndRenderPass() noexcept override;

        void Release() noexcept;

        [[nodiscard]] const IGPUDevice*     GetDevice() const noexcept override;
        [[nodiscard]] EGPUTextureFormat     GetActiveTargetFormat() const noexcept override;
        [[nodiscard]] const GPURect&        GetActiveTargetBounds() const noexcept override;
        [[nodiscard]] const GPUViewport&    GetActiveViewport() const noexcept override;
        [[nodiscard]] SDL_GPUCommandBuffer* GetSDLCommandBuffer() const noexcept;

    private:
        [[nodiscard]] bool EnsureRenderPass() noexcept;

        static constexpr USize ROOT_CONSTANT_BYTE_SIZE = ROOT_CONSTANT_COUNT * sizeof(UInt32);

        const GPUDeviceImpl*                      device             = nullptr;
        SDL_GPUCommandBuffer*                     commandBuffer      = nullptr;
        SDL_GPURenderPass*                        renderPass         = nullptr;
        const GPUGraphicsPipelineStateImpl*       boundPipeline      = nullptr;
        GPUTarget                                 activeTarget       = {};
        bool                                      hasActiveTarget    = false;
        GPURect                                   activeTargetBounds = {};
        GPUViewport                               activeViewport     = {};
        GPURect                                   pendingScissorRect = {};
        bool                                      hasPendingViewport = false;
        bool                                      hasPendingScissor  = false;
        bool                                      pendingClearColor  = false;
        ColorRGBAFloat                            clearColor         = ColorRGBAFloat::GetBlack();
        bool                                      pendingClearDepth  = false;
        Float32                                   clearDepth         = GPUTarget::DEPTH_CLEAR_VALUE;
        std::array<Byte, ROOT_CONSTANT_BYTE_SIZE> rootConstantsData  = {};
    };

    inline const GPURect& GPUCommandBufferImpl::GetActiveTargetBounds() const noexcept
    {
        return activeTargetBounds;
    }

    inline const GPUViewport& GPUCommandBufferImpl::GetActiveViewport() const noexcept
    {
        return activeViewport;
    }

    inline SDL_GPUCommandBuffer* GPUCommandBufferImpl::GetSDLCommandBuffer() const noexcept
    {
        return commandBuffer;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_COMMAND_BUFFER_IMPL_HPP
