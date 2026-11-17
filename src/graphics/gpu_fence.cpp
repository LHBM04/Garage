#include "garage/graphics/gpu_fence.hpp"

#include <utility>

#include <SDL3/SDL_gpu.h>

#include "garage/core/defines.hpp"

#include "garage/graphics/gpu_device.hpp"

namespace Garage
{
    GPUFence::GPUFence(void* deviceHandle, void* fenceHandle) noexcept
        : deviceHandle(deviceHandle)
        , fenceHandle(fenceHandle)
    {
        ASSERT(this->deviceHandle != nullptr);
    }

    GPUFence::~GPUFence() noexcept
    {
        Release();
    }

    GPUFence::GPUFence(GPUFence&& other) noexcept
        : deviceHandle(std::exchange(other.deviceHandle, nullptr))
        , fenceHandle(std::exchange(other.fenceHandle, nullptr))
        , fenceValue(std::exchange(other.fenceValue, 0))
    {
    }

    GPUFence& GPUFence::operator=(GPUFence&& other) noexcept
    {
        if (this != &other)
        {
            Release();
            deviceHandle = std::exchange(other.deviceHandle, nullptr);
            fenceHandle  = std::exchange(other.fenceHandle, nullptr);
            fenceValue   = std::exchange(other.fenceValue, 0);
        }

        return *this;
    }

    void GPUFence::Wait() const noexcept
    {
        if (deviceHandle == nullptr || fenceHandle == nullptr)
        {
            return;
        }

        auto* sdlDevice = static_cast<SDL_GPUDevice*>(deviceHandle);
        auto* sdlFence  = static_cast<SDL_GPUFence*>(fenceHandle);
        SDL_WaitForGPUFences(sdlDevice, true, &sdlFence, 1);
    }

    void GPUFence::Signal(const GPUDevice& device) noexcept
    {
        ASSERT(device.GetNativeDevice() == deviceHandle);
        ++fenceValue;
    }

    void GPUFence::BindNativeFence(void* newFenceHandle) noexcept
    {
        if (deviceHandle != nullptr && fenceHandle != nullptr && fenceHandle != newFenceHandle)
        {
            SDL_ReleaseGPUFence(static_cast<SDL_GPUDevice*>(deviceHandle), static_cast<SDL_GPUFence*>(fenceHandle));
        }

        fenceHandle = newFenceHandle;
        ++fenceValue;
    }

    bool GPUFence::IsCompleted() const noexcept
    {
        if (deviceHandle == nullptr || fenceHandle == nullptr)
        {
            return true;
        }

        return SDL_QueryGPUFence(static_cast<SDL_GPUDevice*>(deviceHandle), static_cast<SDL_GPUFence*>(fenceHandle));
    }

    void GPUFence::Release() noexcept
    {
        if (deviceHandle != nullptr && fenceHandle != nullptr)
        {
            SDL_ReleaseGPUFence(static_cast<SDL_GPUDevice*>(deviceHandle), static_cast<SDL_GPUFence*>(fenceHandle));
        }

        fenceHandle = nullptr;
    }
} // namespace Garage
