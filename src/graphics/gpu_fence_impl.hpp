#ifndef GARAGE_GRAPHICS_GPU_FENCE_IMPL_HPP
#define GARAGE_GRAPHICS_GPU_FENCE_IMPL_HPP

#include <SDL3/SDL_gpu.h>

#include "garage/graphics/gpu_fence.hpp"

namespace Garage
{
    class GPUDeviceImpl;

    class GPUFenceImpl final : public IGPUFence
    {
    public:
        explicit GPUFenceImpl(const GPUDeviceImpl* device) noexcept;
        ~GPUFenceImpl() noexcept override;

        void Wait() noexcept override;
        void Signal(const IGPUDevice& targetDevice) noexcept override;

        void SetSDLFence(SDL_GPUFence* newFence) noexcept;
        void Release() noexcept;

        [[nodiscard]] const IGPUDevice* GetDevice() const noexcept override;
        [[nodiscard]] bool              IsCompleted() const noexcept override;
        [[nodiscard]] SDL_GPUFence*     GetSDLFence() const noexcept;

    private:
        const GPUDeviceImpl* device = nullptr;
        SDL_GPUFence*        fence  = nullptr;
    };

    inline SDL_GPUFence* GPUFenceImpl::GetSDLFence() const noexcept
    {
        return fence;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_FENCE_IMPL_HPP
