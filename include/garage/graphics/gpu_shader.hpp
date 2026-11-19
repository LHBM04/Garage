#ifndef GARAGE_GRAPHICS_GPU_SHADER_HPP
#define GARAGE_GRAPHICS_GPU_SHADER_HPP

#include <span>
#include <string_view>

#include "../core/common.hpp"

#include "gpu_types.hpp"

namespace Garage
{
    class IGPUDevice;

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

    class GARAGE_API IGPUShader
    {
    public:
        IGPUShader() noexcept          = default;
        virtual ~IGPUShader() noexcept = default;

        IGPUShader(const IGPUShader&)            = delete;
        IGPUShader& operator=(const IGPUShader&) = delete;

        IGPUShader(IGPUShader&&)            = delete;
        IGPUShader& operator=(IGPUShader&&) = delete;

        [[nodiscard]] virtual const IGPUDevice* GetDevice() const noexcept = 0;
        [[nodiscard]] virtual EGPUShaderStage   GetStage() const noexcept  = 0;
        [[nodiscard]] virtual EGPUShaderFormat  GetFormat() const noexcept = 0;
    };
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_SHADER_HPP
