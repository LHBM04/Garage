#ifndef GARAGE_GRAPHICS_GPU_SWAP_CHAIN_IMPL_HPP
#define GARAGE_GRAPHICS_GPU_SWAP_CHAIN_IMPL_HPP

#include "garage/core/common.hpp"
#include "garage/core/common/containers.hpp"
#include "garage/core/memory.hpp"

#include "garage/graphics/gpu_swap_chain.hpp"
#include "garage/graphics/gpu_target.hpp"
#include "garage/graphics/gpu_types.hpp"

namespace Garage
{
    class GPUDeviceImpl;
    class GPUTexture2DImpl;
    class IDisplay;
    class IGPUCommandBuffer;
    class IGPUDevice;
    class IGPUTexture2D;

    class GPUSwapChainImpl final : public IGPUSwapChain
    {
    public:
        GPUSwapChainImpl(GPUDeviceImpl* device, const IDisplay* display) noexcept;
        ~GPUSwapChainImpl() noexcept override;

        void Present() const noexcept override;
        void Release() noexcept;

        [[nodiscard]] Result<const GPUTarget*, String>
                                        AcquireBackBufferTarget(IGPUCommandBuffer& commandBuffer) override;
        [[nodiscard]] const GPUTarget*  GetCurrentBackBufferTarget() const noexcept override;
        [[nodiscard]] const IGPUDevice* GetDevice() const noexcept override;
        [[nodiscard]] const IDisplay*   GetDisplay() const noexcept override;
        [[nodiscard]] EGPUTextureFormat GetBackBufferFormat() const noexcept override;

    private:
        GPUDeviceImpl*              device                     = nullptr;
        const IDisplay*             display                    = nullptr;
        EGPUTextureFormat           backBufferFormat           = EGPUTextureFormat::UNKNOWN;
        UniquePtr<GPUTexture2DImpl> backBufferTexture          = nullptr;
        IGPUTexture2D*              depthStencilTexture        = nullptr;
        GPUTarget                   currentBackBufferTarget    = {};
        bool                        hasCurrentBackBufferTarget = false;
    };

    inline const GPUTarget* GPUSwapChainImpl::GetCurrentBackBufferTarget() const noexcept
    {
        return hasCurrentBackBufferTarget ? &currentBackBufferTarget : nullptr;
    }

    inline const IDisplay* GPUSwapChainImpl::GetDisplay() const noexcept
    {
        return display;
    }

    inline EGPUTextureFormat GPUSwapChainImpl::GetBackBufferFormat() const noexcept
    {
        return backBufferFormat;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_SWAP_CHAIN_IMPL_HPP
