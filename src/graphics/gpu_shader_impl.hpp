#ifndef GARAGE_GRAPHICS_GPU_SHADER_IMPL_HPP
#define GARAGE_GRAPHICS_GPU_SHADER_IMPL_HPP

#include <SDL3/SDL_gpu.h>

#include "garage/graphics/gpu_shader.hpp"
#include "garage/graphics/gpu_types.hpp"

namespace Garage
{
    class GPUDeviceImpl;

    class GPUShaderImpl final : public IGPUShader
    {
    public:
        GPUShaderImpl(const GPUDeviceImpl* device,
                      SDL_GPUShader*       shader,
                      EGPUShaderStage      stage,
                      EGPUShaderFormat     format) noexcept;
        ~GPUShaderImpl() noexcept override;

        void Release() noexcept;

        [[nodiscard]] const IGPUDevice* GetDevice() const noexcept override;
        [[nodiscard]] EGPUShaderStage   GetStage() const noexcept override;
        [[nodiscard]] EGPUShaderFormat  GetFormat() const noexcept override;
        [[nodiscard]] SDL_GPUShader*    GetSDLShader() const noexcept;

    private:
        const GPUDeviceImpl* device = nullptr;
        SDL_GPUShader*       shader = nullptr;
        EGPUShaderStage      stage  = EGPUShaderStage::VERTEX;
        EGPUShaderFormat     format = EGPUShaderFormat::INVALID;
    };

    inline EGPUShaderStage GPUShaderImpl::GetStage() const noexcept
    {
        return stage;
    }

    inline EGPUShaderFormat GPUShaderImpl::GetFormat() const noexcept
    {
        return format;
    }

    inline SDL_GPUShader* GPUShaderImpl::GetSDLShader() const noexcept
    {
        return shader;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_SHADER_IMPL_HPP
