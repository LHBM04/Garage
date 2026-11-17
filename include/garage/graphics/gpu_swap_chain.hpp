#ifndef GARAGE_GRAPHICS_GPU_SWAP_CHAIN_HPP
#define GARAGE_GRAPHICS_GPU_SWAP_CHAIN_HPP

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "gpu_target.hpp"
#include "gpu_texture2d.hpp"
#include "gpu_types.hpp"

namespace Garage
{
    class GPUCommandBuffer;
    class IDisplay;

    struct GARAGE_API GPUSwapChainCreateInfo final
    {
        void*             deviceHandle       = nullptr;
        void*             windowHandle       = nullptr;
        const IDisplay*   display            = nullptr;
        EGPUTextureFormat backBufferFormat   = EGPUTextureFormat::R8G8B8A8_UNORM;
        EGPUTextureFormat depthStencilFormat = EGPUTextureFormat::D32_FLOAT;
        UInt32            width              = 0;
        UInt32            height             = 0;
    };

    class GARAGE_API GPUSwapChain final
    {
    public:
        explicit GPUSwapChain(const GPUSwapChainCreateInfo& createInfo) noexcept;
        ~GPUSwapChain() noexcept;

        GPUSwapChain(const GPUSwapChain&)            = delete;
        GPUSwapChain& operator=(const GPUSwapChain&) = delete;

        GPUSwapChain(GPUSwapChain&& other) noexcept;
        GPUSwapChain& operator=(GPUSwapChain&& other) noexcept;

        void Present() const noexcept;

        [[nodiscard]] const GPUTarget*  AcquireBackBufferTarget(GPUCommandBuffer& commandBuffer) noexcept;
        [[nodiscard]] const GPUTarget*  GetCurrentBackBufferTarget() const noexcept;
        [[nodiscard]] const IDisplay*   GetDisplay() const noexcept;
        [[nodiscard]] EGPUTextureFormat GetBackBufferFormat() const noexcept;

    private:
        void               Release() noexcept;
        [[nodiscard]] bool RecreateDepthStencil(UInt32 newWidth, UInt32 newHeight) noexcept;

        void*             deviceHandle       = nullptr;
        void*             windowHandle       = nullptr;
        const IDisplay*   display            = nullptr;
        EGPUTextureFormat backBufferFormat   = EGPUTextureFormat::R8G8B8A8_UNORM;
        EGPUTextureFormat depthStencilFormat = EGPUTextureFormat::D32_FLOAT;
        GPUTexture2D      backBufferTexture  = {};
        GPUTexture2D      depthStencil       = {};
        GPUTarget         currentTarget      = {};
        bool              hasValidTarget     = false;
    };

    inline const GPUTarget* GPUSwapChain::GetCurrentBackBufferTarget() const noexcept
    {
        return hasValidTarget ? &currentTarget : nullptr;
    }

    inline const IDisplay* GPUSwapChain::GetDisplay() const noexcept
    {
        return display;
    }

    inline EGPUTextureFormat GPUSwapChain::GetBackBufferFormat() const noexcept
    {
        return backBufferFormat;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_SWAP_CHAIN_HPP
