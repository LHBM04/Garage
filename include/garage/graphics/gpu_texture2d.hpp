#ifndef GARAGE_GRAPHICS_GPU_TEXTURE2D_HPP
#define GARAGE_GRAPHICS_GPU_TEXTURE2D_HPP

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "gpu_types.hpp"

namespace Garage
{
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

    struct GARAGE_API GPUTexture2DHandleInfo final
    {
        void*             deviceHandle  = nullptr;
        void*             textureHandle = nullptr;
        EGPUTextureType   type          = EGPUTextureType::TEXTURE_2D;
        EGPUTextureFormat format        = EGPUTextureFormat::R8G8B8A8_UNORM;
        UInt32            width         = 0;
        UInt32            height        = 0;
        UInt32            layerCount    = 1;
        UInt32            mipLevels     = 1;
        bool              ownsHandle    = true;
    };

    class GARAGE_API GPUTexture2D final
    {
    public:
        GPUTexture2D() noexcept = default;
        explicit GPUTexture2D(const GPUTexture2DHandleInfo& handleInfo) noexcept;
        ~GPUTexture2D() noexcept;

        GPUTexture2D(const GPUTexture2D&)            = delete;
        GPUTexture2D& operator=(const GPUTexture2D&) = delete;

        GPUTexture2D(GPUTexture2D&& other) noexcept;
        GPUTexture2D& operator=(GPUTexture2D&& other) noexcept;

        void ResetBorrowedHandle(void*             newTextureHandle,
                                 EGPUTextureFormat newFormat,
                                 UInt32            newWidth,
                                 UInt32            newHeight) noexcept;

        [[nodiscard]] UInt32            GetWidth() const noexcept;
        [[nodiscard]] UInt32            GetHeight() const noexcept;
        [[nodiscard]] UInt32            GetLayerCount() const noexcept;
        [[nodiscard]] UInt32            GetMipLevels() const noexcept;
        [[nodiscard]] EGPUTextureFormat GetFormat() const noexcept;
        [[nodiscard]] EGPUTextureType   GetType() const noexcept;
        [[nodiscard]] void*             GetNativeHandle() const noexcept;

    private:
        void Release() noexcept;

        void*             deviceHandle  = nullptr;
        void*             textureHandle = nullptr;
        EGPUTextureType   type          = EGPUTextureType::TEXTURE_2D;
        EGPUTextureFormat format        = EGPUTextureFormat::UNKNOWN;
        UInt32            width         = 0;
        UInt32            height        = 0;
        UInt32            layerCount    = 1;
        UInt32            mipLevels     = 1;
        bool              ownsHandle    = false;
    };

    inline UInt32 GPUTexture2D::GetWidth() const noexcept
    {
        return width;
    }

    inline UInt32 GPUTexture2D::GetHeight() const noexcept
    {
        return height;
    }

    inline UInt32 GPUTexture2D::GetLayerCount() const noexcept
    {
        return layerCount;
    }

    inline UInt32 GPUTexture2D::GetMipLevels() const noexcept
    {
        return mipLevels;
    }

    inline EGPUTextureFormat GPUTexture2D::GetFormat() const noexcept
    {
        return format;
    }

    inline EGPUTextureType GPUTexture2D::GetType() const noexcept
    {
        return type;
    }

    inline void* GPUTexture2D::GetNativeHandle() const noexcept
    {
        return textureHandle;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_TEXTURE2D_HPP
