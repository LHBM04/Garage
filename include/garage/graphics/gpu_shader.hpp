#ifndef GARAGE_GRAPHICS_GPU_SHADER_HPP
#define GARAGE_GRAPHICS_GPU_SHADER_HPP

#include <span>
#include <string_view>

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "gpu_types.hpp"

namespace Garage
{
    struct GARAGE_API GPUShaderCreateInfo final
    {
        std::span<const Byte> code                = {};
        std::string_view      entryPoint          = "main";
        EGPUShaderFormat      format              = EGPUShaderFormat::INVALID;
        EGPUShaderStage       stage               = EGPUShaderStage::VERTEX;
        UInt32                samplerCount        = 0;
        UInt32                storageTextureCount = 0;
        UInt32                storageBufferCount  = 0;
        UInt32                uniformBufferCount  = 0;
    };

    class GARAGE_API GPUShader final
    {
    public:
        GPUShader(void* deviceHandle, void* shaderHandle, EGPUShaderStage stage, EGPUShaderFormat format) noexcept;
        ~GPUShader() noexcept;

        GPUShader(const GPUShader&)            = delete;
        GPUShader& operator=(const GPUShader&) = delete;

        GPUShader(GPUShader&& other) noexcept;
        GPUShader& operator=(GPUShader&& other) noexcept;

        [[nodiscard]] EGPUShaderStage  GetStage() const noexcept;
        [[nodiscard]] EGPUShaderFormat GetFormat() const noexcept;
        [[nodiscard]] void*            GetNativeHandle() const noexcept;

    private:
        void Release() noexcept;

        void*            deviceHandle = nullptr;
        void*            shaderHandle = nullptr;
        EGPUShaderStage  stage        = EGPUShaderStage::VERTEX;
        EGPUShaderFormat format       = EGPUShaderFormat::INVALID;
    };

    inline EGPUShaderStage GPUShader::GetStage() const noexcept
    {
        return stage;
    }

    inline EGPUShaderFormat GPUShader::GetFormat() const noexcept
    {
        return format;
    }

    inline void* GPUShader::GetNativeHandle() const noexcept
    {
        return shaderHandle;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_SHADER_HPP
