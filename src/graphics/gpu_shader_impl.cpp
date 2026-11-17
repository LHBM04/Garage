#include "gpu_shader_impl.hpp"

#include <SDL3/SDL_gpu.h>

#include "garage/graphics/gpu_device.hpp"

#include "gpu_device_impl.hpp"

namespace Garage
{
    GPUShaderImpl::GPUShaderImpl(const GPUDeviceImpl* device,
                                 SDL_GPUShader*       shader,
                                 EGPUShaderStage      stage,
                                 EGPUShaderFormat     format) noexcept
        : device(device)
        , shader(shader)
        , stage(stage)
        , format(format)
    {
    }

    GPUShaderImpl::~GPUShaderImpl() noexcept
    {
        Release();
    }

    void GPUShaderImpl::Release() noexcept
    {
        if (shader != nullptr && device != nullptr && device->GetSDLDevice() != nullptr)
        {
            SDL_ReleaseGPUShader(device->GetSDLDevice(), shader);
        }

        shader = nullptr;
        device = nullptr;
    }

    const IGPUDevice* GPUShaderImpl::GetDevice() const noexcept
    {
        return device;
    }
} // namespace Garage
