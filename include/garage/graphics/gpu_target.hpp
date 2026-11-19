#ifndef GARAGE_GRAPHICS_GPU_TARGET_HPP
#define GARAGE_GRAPHICS_GPU_TARGET_HPP

#include "../core/common.hpp"

#include "gpu_types.hpp"

namespace Garage
{
    class IGPUTexture2D;

    struct GARAGE_API GPUTargetCreateInfo final
    {
        const IGPUTexture2D* texture             = nullptr;
        const IGPUTexture2D* depthStencilTexture = nullptr;
        EGPUTextureFormat    format              = EGPUTextureFormat::UNKNOWN;
    };

    class GARAGE_API GPUTarget final
    {
    public:
        static constexpr EGPUTextureFormat DEPTH_FORMAT      = EGPUTextureFormat::D32_FLOAT;
        static constexpr Float32           DEPTH_CLEAR_VALUE = 1.0f;

        GPUTarget() noexcept = default;
        explicit GPUTarget(const GPUTargetCreateInfo& createInfo) noexcept;
        ~GPUTarget() noexcept = default;

        GPUTarget(const GPUTarget&) noexcept            = default;
        GPUTarget& operator=(const GPUTarget&) noexcept = default;

        GPUTarget(GPUTarget&&) noexcept            = default;
        GPUTarget& operator=(GPUTarget&&) noexcept = default;

        [[nodiscard]] const IGPUTexture2D* GetTexture() const noexcept;
        [[nodiscard]] const IGPUTexture2D* GetDepthStencilTexture() const noexcept;
        [[nodiscard]] EGPUTextureFormat    GetFormat() const noexcept;

    private:
        const IGPUTexture2D* texture             = nullptr;
        const IGPUTexture2D* depthStencilTexture = nullptr;
        EGPUTextureFormat    format              = EGPUTextureFormat::UNKNOWN;
    };

    inline GPUTarget::GPUTarget(const GPUTargetCreateInfo& createInfo) noexcept
        : texture(createInfo.texture)
        , depthStencilTexture(createInfo.depthStencilTexture)
        , format(createInfo.format)
    {
        ASSERT(this->texture != nullptr);
    }

    inline const IGPUTexture2D* GPUTarget::GetTexture() const noexcept
    {
        return texture;
    }

    inline const IGPUTexture2D* GPUTarget::GetDepthStencilTexture() const noexcept
    {
        return depthStencilTexture;
    }

    inline EGPUTextureFormat GPUTarget::GetFormat() const noexcept
    {
        return format;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_TARGET_HPP
