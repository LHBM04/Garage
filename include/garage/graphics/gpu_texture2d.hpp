#ifndef GARAGE_GRAPHICS_GPU_TEXTURE2D_HPP
#define GARAGE_GRAPHICS_GPU_TEXTURE2D_HPP

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "gpu_types.hpp"

namespace Garage
{
    class IGPUDevice;

    struct GARAGE_API GPUTexture2DCreateInfo final
    {
        EGPUTextureType   type       = EGPUTextureType::TEXTURE_2D;
        EGPUTextureFormat format     = EGPUTextureFormat::R8G8B8A8_UNORM;
        UInt32            usage      = static_cast<UInt32>(EGPUTextureUsage::SAMPLER);
        UInt32            width      = 1;
        UInt32            height     = 1;
        UInt32            layerCount = 1;
        UInt32            mipLevels  = 1;
    };

    class GARAGE_API IGPUTexture2D
    {
    public:
        IGPUTexture2D() noexcept          = default;
        virtual ~IGPUTexture2D() noexcept = default;

        IGPUTexture2D(const IGPUTexture2D&)            = delete;
        IGPUTexture2D& operator=(const IGPUTexture2D&) = delete;

        IGPUTexture2D(IGPUTexture2D&&)            = delete;
        IGPUTexture2D& operator=(IGPUTexture2D&&) = delete;

        [[nodiscard]] virtual const IGPUDevice* GetDevice() const noexcept     = 0;
        [[nodiscard]] virtual UInt32            GetWidth() const noexcept      = 0;
        [[nodiscard]] virtual UInt32            GetHeight() const noexcept     = 0;
        [[nodiscard]] virtual UInt32            GetLayerCount() const noexcept = 0;
        [[nodiscard]] virtual UInt32            GetMipLevels() const noexcept  = 0;
        [[nodiscard]] virtual EGPUTextureFormat GetFormat() const noexcept     = 0;
        [[nodiscard]] virtual EGPUTextureType   GetType() const noexcept       = 0;
    };
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_TEXTURE2D_HPP
