#ifndef GARAGE_GRAPHICS_GPU_TEXTURE2D_IMPL_HPP
#define GARAGE_GRAPHICS_GPU_TEXTURE2D_IMPL_HPP

#include <SDL3/SDL_gpu.h>

#include "garage/core/types.hpp"

#include "garage/graphics/gpu_texture2d.hpp"
#include "garage/graphics/gpu_types.hpp"

namespace Garage
{
    class GPUDeviceImpl;

    class GPUTexture2DImpl final : public IGPUTexture2D
    {
    public:
        GPUTexture2DImpl(const GPUDeviceImpl* device,
                         SDL_GPUTexture*      texture,
                         UInt32               width,
                         UInt32               height,
                         UInt32               layerCount,
                         UInt32               mipLevels,
                         EGPUTextureFormat    format,
                         EGPUTextureType      type,
                         bool                 ownsHandle) noexcept;
        ~GPUTexture2DImpl() noexcept override;

        void ResetBorrowedHandle(SDL_GPUTexture*   newTexture,
                                 UInt32            newWidth,
                                 UInt32            newHeight,
                                 EGPUTextureFormat newFormat) noexcept;
        void Release() noexcept;

        [[nodiscard]] const IGPUDevice* GetDevice() const noexcept override;
        [[nodiscard]] UInt32            GetWidth() const noexcept override;
        [[nodiscard]] UInt32            GetHeight() const noexcept override;
        [[nodiscard]] UInt32            GetLayerCount() const noexcept override;
        [[nodiscard]] UInt32            GetMipLevels() const noexcept override;
        [[nodiscard]] EGPUTextureFormat GetFormat() const noexcept override;
        [[nodiscard]] EGPUTextureType   GetType() const noexcept override;
        [[nodiscard]] SDL_GPUTexture*   GetSDLTexture() const noexcept;

    private:
        const GPUDeviceImpl* device     = nullptr;
        SDL_GPUTexture*      texture    = nullptr;
        UInt32               width      = 0;
        UInt32               height     = 0;
        UInt32               layerCount = 1;
        UInt32               mipLevels  = 1;
        EGPUTextureFormat    format     = EGPUTextureFormat::UNKNOWN;
        EGPUTextureType      type       = EGPUTextureType::TEXTURE_2D;
        bool                 ownsHandle = true;
    };

    inline UInt32 GPUTexture2DImpl::GetWidth() const noexcept
    {
        return width;
    }

    inline UInt32 GPUTexture2DImpl::GetHeight() const noexcept
    {
        return height;
    }

    inline UInt32 GPUTexture2DImpl::GetLayerCount() const noexcept
    {
        return layerCount;
    }

    inline UInt32 GPUTexture2DImpl::GetMipLevels() const noexcept
    {
        return mipLevels;
    }

    inline EGPUTextureFormat GPUTexture2DImpl::GetFormat() const noexcept
    {
        return format;
    }

    inline EGPUTextureType GPUTexture2DImpl::GetType() const noexcept
    {
        return type;
    }

    inline SDL_GPUTexture* GPUTexture2DImpl::GetSDLTexture() const noexcept
    {
        return texture;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_TEXTURE2D_IMPL_HPP
