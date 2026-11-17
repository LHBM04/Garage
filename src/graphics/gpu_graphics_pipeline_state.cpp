#include "garage/graphics/gpu_graphics_pipeline_state.hpp"

#include <utility>

#include <SDL3/SDL_gpu.h>

#include "garage/core/defines.hpp"

namespace Garage
{
    GPUGraphicsPipelineState::GPUGraphicsPipelineState(void* deviceHandle, void* pipelineHandle) noexcept
        : deviceHandle(deviceHandle)
        , pipelineHandle(pipelineHandle)
    {
        ASSERT(this->deviceHandle != nullptr);
        ASSERT(this->pipelineHandle != nullptr);
    }

    GPUGraphicsPipelineState::~GPUGraphicsPipelineState() noexcept
    {
        Release();
    }

    GPUGraphicsPipelineState::GPUGraphicsPipelineState(GPUGraphicsPipelineState&& other) noexcept
        : deviceHandle(std::exchange(other.deviceHandle, nullptr))
        , pipelineHandle(std::exchange(other.pipelineHandle, nullptr))
    {
    }

    GPUGraphicsPipelineState& GPUGraphicsPipelineState::operator=(GPUGraphicsPipelineState&& other) noexcept
    {
        if (this != &other)
        {
            Release();
            deviceHandle   = std::exchange(other.deviceHandle, nullptr);
            pipelineHandle = std::exchange(other.pipelineHandle, nullptr);
        }

        return *this;
    }

    void GPUGraphicsPipelineState::Release() noexcept
    {
        if (deviceHandle != nullptr && pipelineHandle != nullptr)
        {
            SDL_ReleaseGPUGraphicsPipeline(static_cast<SDL_GPUDevice*>(deviceHandle),
                                           static_cast<SDL_GPUGraphicsPipeline*>(pipelineHandle));
        }

        deviceHandle   = nullptr;
        pipelineHandle = nullptr;
    }
} // namespace Garage
