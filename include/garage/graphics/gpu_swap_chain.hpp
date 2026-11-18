#ifndef GARAGE_GRAPHICS_GPU_SWAP_CHAIN_HPP
#define GARAGE_GRAPHICS_GPU_SWAP_CHAIN_HPP

#include <string>

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "gpu_types.hpp"

namespace Garage
{
    class GPUTarget;
    class IDisplay;
    class IGPUCommandBuffer;
    class IGPUDevice;

    class GARAGE_API IGPUSwapChain
    {
    public:
        IGPUSwapChain() noexcept          = default;
        virtual ~IGPUSwapChain() noexcept = default;

        IGPUSwapChain(const IGPUSwapChain&)            = delete;
        IGPUSwapChain& operator=(const IGPUSwapChain&) = delete;

        IGPUSwapChain(IGPUSwapChain&&)            = delete;
        IGPUSwapChain& operator=(IGPUSwapChain&&) = delete;

        virtual void Present() const noexcept = 0;

        [[nodiscard]] virtual Result<const GPUTarget*, std::string>
                                                AcquireBackBufferTarget(IGPUCommandBuffer& commandBuffer) = 0;
        [[nodiscard]] virtual const GPUTarget*  GetCurrentBackBufferTarget() const noexcept               = 0;
        [[nodiscard]] virtual const IGPUDevice* GetDevice() const noexcept                                = 0;
        [[nodiscard]] virtual const IDisplay*   GetDisplay() const noexcept                               = 0;
        [[nodiscard]] virtual EGPUTextureFormat GetBackBufferFormat() const noexcept                      = 0;
    };
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_SWAP_CHAIN_HPP
