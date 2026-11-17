#include "garage/graphics/gpu_shader.hpp"

#include <utility>

#include <SDL3/SDL_gpu.h>

#include "garage/core/defines.hpp"

#include "garage/graphics/gpu_types.hpp"

namespace Garage
{
    GPUShader::GPUShader(void*            deviceHandle,
                         void*            shaderHandle,
                         EGPUShaderStage  stage,
                         EGPUShaderFormat format) noexcept
        : deviceHandle(deviceHandle)
        , shaderHandle(shaderHandle)
        , stage(stage)
        , format(format)
    {
        ASSERT(this->deviceHandle != nullptr);
        ASSERT(this->shaderHandle != nullptr);
    }

    GPUShader::~GPUShader() noexcept
    {
        Release();
    }

    GPUShader::GPUShader(GPUShader&& other) noexcept
        : deviceHandle(std::exchange(other.deviceHandle, nullptr))
        , shaderHandle(std::exchange(other.shaderHandle, nullptr))
        , stage(other.stage)
        , format(other.format)
    {
    }

    GPUShader& GPUShader::operator=(GPUShader&& other) noexcept
    {
        if (this != &other)
        {
            Release();
            deviceHandle = std::exchange(other.deviceHandle, nullptr);
            shaderHandle = std::exchange(other.shaderHandle, nullptr);
            stage        = other.stage;
            format       = other.format;
        }

        return *this;
    }

    void GPUShader::Release() noexcept
    {
        if (deviceHandle != nullptr && shaderHandle != nullptr)
        {
            SDL_ReleaseGPUShader(static_cast<SDL_GPUDevice*>(deviceHandle), static_cast<SDL_GPUShader*>(shaderHandle));
        }

        deviceHandle = nullptr;
        shaderHandle = nullptr;
    }
} // namespace Garage
