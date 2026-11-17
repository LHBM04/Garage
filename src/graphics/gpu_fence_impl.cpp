#include "gpu_fence_impl.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>

#include "garage/graphics/gpu_device.hpp"

#include "garage/utilities/logger.hpp"

#include "gpu_device_impl.hpp"

namespace Garage
{
    GPUFenceImpl::GPUFenceImpl(const GPUDeviceImpl* device) noexcept
        : device(device)
    {
    }

    GPUFenceImpl::~GPUFenceImpl() noexcept
    {
        Release();
    }

    void GPUFenceImpl::Wait() const noexcept
    {
        if (fence == nullptr || device == nullptr || device->GetSDLDevice() == nullptr)
        {
            return;
        }

        SDL_GPUDevice* sdlDevice = device->GetSDLDevice();
        SDL_WaitForGPUFences(sdlDevice, true, &fence, 1);
        SDL_ReleaseGPUFence(sdlDevice, fence);
        fence = nullptr;
    }

    void GPUFenceImpl::Signal(const IGPUDevice& targetDevice) noexcept
    {
        const auto& targetImpl = static_cast<const GPUDeviceImpl&>(targetDevice);
        if (targetImpl.GetSDLDevice() == nullptr)
        {
            return;
        }

        Release();
        device = &targetImpl;

        SDL_GPUDevice*        sdlDevice        = device->GetSDLDevice();
        SDL_GPUCommandBuffer* sdlCommandBuffer = SDL_AcquireGPUCommandBuffer(sdlDevice);
        if (sdlCommandBuffer == nullptr)
        {
            LOG_ERROR("GPU 펜스 시그널용 명령 버퍼 획득에 실패했습니다: {}", SDL_GetError());
            return;
        }

        SDL_GPUFence* sdlFence = SDL_SubmitGPUCommandBufferAndAcquireFence(sdlCommandBuffer);
        if (sdlFence == nullptr)
        {
            LOG_ERROR("GPU 펜스 시그널 제출에 실패했습니다: {}", SDL_GetError());
            return;
        }

        fence = sdlFence;
    }

    void GPUFenceImpl::SetSDLFence(SDL_GPUFence* newFence) noexcept
    {
        if (fence != nullptr && device != nullptr && device->GetSDLDevice() != nullptr)
        {
            SDL_ReleaseGPUFence(device->GetSDLDevice(), fence);
        }

        fence = newFence;
    }

    void GPUFenceImpl::Release() noexcept
    {
        if (fence != nullptr && device != nullptr && device->GetSDLDevice() != nullptr)
        {
            SDL_ReleaseGPUFence(device->GetSDLDevice(), fence);
        }

        fence = nullptr;
    }

    const IGPUDevice* GPUFenceImpl::GetDevice() const noexcept
    {
        return device;
    }

    bool GPUFenceImpl::IsCompleted() const noexcept
    {
        if (fence == nullptr || device == nullptr || device->GetSDLDevice() == nullptr)
        {
            return true;
        }

        return SDL_QueryGPUFence(device->GetSDLDevice(), fence);
    }
} // namespace Garage
