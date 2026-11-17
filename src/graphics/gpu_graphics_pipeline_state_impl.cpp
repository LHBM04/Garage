#include "gpu_graphics_pipeline_state_impl.hpp"

#include <SDL3/SDL_gpu.h>

#include "garage/graphics/gpu_device.hpp"

#include "gpu_device_impl.hpp"

namespace Garage
{
    GPUGraphicsPipelineStateImpl::GPUGraphicsPipelineStateImpl(const GPUDeviceImpl*     device,
                                                               SDL_GPUGraphicsPipeline* pipelineState) noexcept
        : device(device)
        , pipelineState(pipelineState)
    {
    }

    GPUGraphicsPipelineStateImpl::~GPUGraphicsPipelineStateImpl() noexcept
    {
        Release();
    }

    void GPUGraphicsPipelineStateImpl::Release() noexcept
    {
        if (pipelineState != nullptr && device != nullptr && device->GetSDLDevice() != nullptr)
        {
            SDL_ReleaseGPUGraphicsPipeline(device->GetSDLDevice(), pipelineState);
        }

        pipelineState = nullptr;
        device        = nullptr;
    }

    const IGPUDevice* GPUGraphicsPipelineStateImpl::GetDevice() const noexcept
    {
        return device;
    }
} // namespace Garage
